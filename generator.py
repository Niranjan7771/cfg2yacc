#!/usr/bin/env python3
"""
generator.py
---------------
Entry-point for converting a `.def` analyzer description into a Flex (`lexer.l`) and
Bison (`parser.y`) input files and (optionally) token example files. This script is
used by the Makefile to produce the C sources that are compiled into the final
`custom_compiler` binary.

High level responsibilities:
- Parse the `.def` file format which contains two sections: `%%LEX` and `%%YACC`.
- Produce `lexer.l` (Flex) from the lex rules in `%%LEX`.
- Produce `parser.y` (Bison) from grammar rules in `%%YACC` and generate %token/%type
    declarations.
- Generate example token files for samples (historically via `TOKEN_TEMPLATES`,
    optionally by scanning the sample input).

This file contains small helper classes `LexRule` and `GrammarRule` used by the
generator. Each top-level function includes a short docstring describing its
inputs/outputs and behaviour.
"""

import sys
import os
import re
from pathlib import Path
from typing import List, Tuple

# Token data templates - comprehensive examples for auto-generation
# NOTE: Historically the repository supported two modes of token generation:
# 1) Template-driven: write token example files from TOKEN_TEMPLATES (fast, static)
# 2) Input-driven: scan the sample input file and extract tokens that match the
#    lex regexes (dynamic, input-driven). The current TOKEN_TEMPLATES dictionary
# supports mode (1). To switch to input-driven mode edit `generate_token_files`.
TOKEN_TEMPLATES = {
    'EMAIL': ['john.doe@example.com', 'alice@test.org', 'support@company.net'],
    'EMAIL_ADDR': ['john.doe@company.com', 'support@bank.com', 'info@site.org'],
    'PHONE_NUMBER': ['555-123-4567', '800-555-0199', '555-987-6543'],
    'PHONE_FORMATTED': ['(555) 123-4567', '(800) 555-0199', '(212) 555-1234'],
    'PHONE_SIMPLE': ['555-987-6543', '123 456 7890', '555.123.4567'],
    'WEBSITE_URL': ['www.example.com', 'www.bank.com', 'www.store.org'],
    'CURRENCY': ['$99.99', '$25.00', '$15.50', '$500.00'],
    'CURRENCY_USD': ['$1,234.56', '$999.99', '$50.00'],
    'DATE_FORMAT': ['12/15/2024', '01/01/2025', '03/14/2024'],
    'DATE_MEDICAL': ['11/03/2025', '10/15/2024', '12/25/2024'],
    'TIME_MEDICAL': ['09:30AM', '02:15PM', '11:45AM'],
    'URGENT': ['URGENT', 'IMMEDIATE', 'ACT NOW', 'LIMITED TIME'],
    'SPAM_WORDS': ['winner', 'free', 'prize', 'congratulations'],
    'MONEY_TERMS': ['cash', 'money', 'profit', 'income'],
    'URGENCY_WORDS': ['urgent', 'immediate', 'now', 'hurry'],
    'SUSPICIOUS_LINK': ['bit.ly/xyz123', 'tinyurl.com/abc'],
    'CAPS_WORD': ['WINNER', 'FREE', 'GUARANTEED'],
    'FLOAT_NUMBER': ['3.14', '2.718', '1.23', '99.99'],
    'NUMBER': ['123', '456', '789', '0', '999'],
    'INTEGER': ['42', '100', '0', '9999'],
    'TIMESTAMP': ['2024-11-03 14:23:45', '2024-11-03 09:15:30'],
    'IP_ADDRESS': ['192.168.1.100', '10.0.0.1', '8.8.8.8'],
    'HTTP_METHOD': ['GET', 'POST', 'PUT', 'DELETE'],
    'STATUS_CODE': ['200', '404', '500', '301'],
    'ERROR_LEVEL': ['ERROR', 'WARNING', 'INFO', 'DEBUG'],
    'HASHTAG': ['#technology', '#ai', '#programming'],
    'USER_MENTION': ['@user123', '@john_doe', '@tech_news'],
    'EMOJI_FACE': ['😀', '😊', '🎉', '❤️'],
    'LIKE_COUNT': ['1.5K likes', '234 likes', '10K likes'],
    'SHARE_COUNT': ['500 shares', '1.2K shares', '50 shares'],
    'ACCOUNT_NUMBER': ['ACC-123456789', 'ACC-987654321'],
    'TRANSACTION_ID': ['TXN-ABC123XYZ', 'TXN-DEF456UVW'],
    'STOCK_SYMBOL': ['AAPL', 'GOOGL', 'MSFT', 'TSLA'],
    'FUNCTION_DEF': ['def calculate():', 'function getData() {'],
    'VARIABLE_ASSIGN': ['x = 10', 'result = func()', 'data = []'],
    'STRING_LITERAL': ['"Hello, World!"', "'test string'"],
    # Medical tokens
    'PATIENT_ID': ['MR234567', 'AB123456', 'CD789012', 'EF456789'],
    'BLOOD_TYPE': ['A+', 'A-', 'B+', 'B-', 'AB+', 'AB-', 'O+', 'O-'],
    'BLOOD_PRESSURE': ['120/80', '130/85', '140/90', '110/70'],
    'TEMPERATURE': ['98.6F', '99.2F', '97.8F', '37.0C'],
    'HEART_RATE': ['72bpm', '78bpm', '85bpm', '95bpm'],
    'WEIGHT': ['75kg', '62kg', '82kg', '165lbs'],
    'HEIGHT': ['1.75m', '1.65m', '180cm', '175cm'],
    'GLUCOSE_LEVEL': ['95mg/dL', '110mg/dL', '88mg/dL'],
    'PRESCRIPTION': ['Rx12345678', 'Rx23456789', 'Rx34567890'],
    'DIAGNOSIS_CODE': ['ICD10-I21', 'ICD10-E11', 'ICD10-G40'],
    'DOCTOR_ID': ['Dr.Smith', 'Dr.Johnson', 'Dr.Williams'],
    'TEST_RESULT': ['Positive', 'Negative', 'Pending', 'Abnormal', 'Normal'],
    'SEVERITY': ['Critical', 'Severe', 'Moderate', 'Mild'],
    'DEPARTMENT': ['Cardiology', 'Neurology', 'Emergency', 'ICU'],
    'GENDER': ['Male', 'Female', 'Other'],
    'AGE_UNIT': ['years', 'yrs'],
    'DOSAGE_FORM': ['tablet', 'tablets', 'capsule', 'capsules', 'pill', 'pills', 'dose', 'doses', 'ml', 'mg'],
}

class LexRule:
    def __init__(self, token_name: str, regex: str):
        self.token_name = token_name
        self.regex = regex

    # LexRule represents a single lexer specification line from the %%LEX
    # section. `token_name` is the symbolic token name and `regex` is the
    # Flex-style pattern string that will be written verbatim into `lexer.l`.

class GrammarRule:
    def __init__(self, lhs: str, rhs: str, action: str):
        self.lhs = lhs
        self.rhs = rhs
        self.action = action

    # GrammarRule holds one production rule extracted from the %%YACC
    # section. `lhs` is the non-terminal on the left hand side, `rhs` is the
    # textual right-hand side (as written in the .def) and `action` contains
    # any `{ ... }` semantic action code the user provided.

def parse_def_file(filename: str) -> Tuple[List[LexRule], List[GrammarRule]]:
    """
    Read and parse a `.def` file.

    Returns two lists:
    - list of `LexRule` objects found in the `%%LEX` section
    - list of `GrammarRule` objects found in the `%%YACC` section

    The parser is intentionally simple: it expects one `%%LEX` and one
    `%%YACC` marker and parses lines pragmatically. Complex, nested or
    multi-line actions are handled in a best-effort manner.
    """

    with open(filename, 'r') as f:
        content = f.read()

    # Basic validation that the required sections exist
    if '%%LEX' not in content or '%%YACC' not in content:
        print(f'Error: {filename} must contain both %%LEX and %%YACC sections')
        sys.exit(1)

    # Split the file into lex and yacc sections. This assumes a single
    # `%%LEX` followed by a single `%%YACC` in that order.
    parts = content.split('%%LEX')
    after_lex = parts[1].split('%%YACC')
    lex_section = after_lex[0]
    yacc_section = after_lex[1]
    
    lex_rules = []
    for line in lex_section.strip().split('\n'):
        line = line.strip()
        if not line or line.startswith('#'):
            continue
        parts = line.split(None, 1)
        if len(parts) == 2:
            token_name, regex = parts
            lex_rules.append(LexRule(token_name, regex))
    
    grammar_rules = []
    lines = yacc_section.strip().split('\n')
    i = 0
    while i < len(lines):
        line = lines[i].strip()
        
        if not line or line.startswith('#'):
            i += 1
            continue
        
        if '->' in line:
            parts = line.split('->', 1)
            lhs = parts[0].strip()
            rest = parts[1].strip()
            
            if '{' in rest:
                brace_content = rest[rest.index('{')+1:]
                rhs = rest[:rest.index('{')].strip()
                action = brace_content[:brace_content.index('}')].strip()
            else:
                rhs = rest
                action = ''
                if i + 1 < len(lines):
                    next_line = lines[i + 1].strip()
                    if next_line.startswith('{'):
                        action = next_line[next_line.index('{')+1:next_line.index('}')].strip()
                        i += 1
            
            grammar_rules.append(GrammarRule(lhs, rhs, action))
        
        elif line.startswith('|'):
            if grammar_rules:
                last_lhs = grammar_rules[-1].lhs
                rest = line[1:].strip()
                
                if '{' in rest:
                    brace_content = rest[rest.index('{')+1:]
                    rhs = rest[:rest.index('{')].strip()
                    action = brace_content[:brace_content.index('}')].strip()
                else:
                    rhs = rest
                    action = ''
                    if i + 1 < len(lines):
                        next_line = lines[i + 1].strip()
                        if next_line.startswith('{'):
                            action = next_line[next_line.index('{')+1:next_line.index('}')].strip()
                            i += 1
                
                grammar_rules.append(GrammarRule(last_lhs, rhs, action))
        
        i += 1
    
    return lex_rules, grammar_rules

def generate_lexer(lex_rules: List[LexRule], output_file: str):
    """
    Emit a Flex `lexer.l` file from parsed `lex_rules`.

    For each `LexRule` we write a Flex pattern line. For regular tokens we
    produce code that creates an AST leaf node (`yylval.node = create_leaf_node(...)`)
    and returns the token symbol to the parser. `WHITESPACE` is treated as a
    skip rule to avoid creating extraneous nodes for whitespace.
    """

    with open(output_file, 'w') as f:
        # C prologue required by flex/bison integration
        f.write('%{\n')
        f.write('#include <stdio.h>\n')
        f.write('#include <stdlib.h>\n')
        f.write('#include <string.h>\n')
        f.write('#include "ast.h"\n')
        f.write('#include "y.tab.h"\n')
        f.write('%}\n\n')
        f.write('%%\n\n')

        # Emit each lexer rule. The `.regex` is written verbatim: the .def
        # author is responsible for Flex-compatible patterns.
        for rule in lex_rules:
            if rule.token_name == 'WHITESPACE':
                # Skip whitespace tokens entirely (no AST node created)
                f.write(rule.regex + '    { /* skip whitespace */ }\n')
            else:
                # For named tokens, create a leaf node and return the token
                f.write(rule.regex + '    { ')
                f.write('yylval.node = create_leaf_node("' + rule.token_name + '", yytext); ')
                f.write('return ' + rule.token_name + '; ')
                f.write('}\n')

        # A fallback rule for unexpected input
        f.write('\n.    { fprintf(stderr, "Unexpected character: %s\\n", yytext); }\n')
        f.write('%%\n\n')
        f.write('int yywrap() { return 1; }\n')

def generate_parser(lex_rules: List[LexRule], grammar_rules: List[GrammarRule], output_file: str):
    """
    Emit a Bison `parser.y` file using the provided `lex_rules` and
    `grammar_rules`.

    The function performs these steps:
    - Writes the C prologue required by Bison and declares `Node *` union.
    - Declares `%token` entries for each lexer token (attaching the `<node>`
      semantic type) and `%type` for nonterminals.
    - Emits the grammar productions verbatim and attaches any user-provided
      action blocks. The first production's action is recorded in `ast_root`.
    - Produces a simple `main()` that opens an optional input file, runs the
      parser, and prints the resulting AST with `print_ast()`.
    """

    with open(output_file, 'w') as f:
        # Bison C prologue: includes and forward declarations
        f.write('%{\n')
        f.write('#include <stdio.h>\n')
        f.write('#include <stdlib.h>\n')
        f.write('#include <string.h>\n')
        f.write('#include "ast.h"\n\n')
        f.write('extern int yylex();\n')
        f.write('extern int yyparse();\n')
        f.write('extern FILE *yyin;\n')
        f.write('void yyerror(const char *s);\n\n')
        f.write('Node *ast_root = NULL;\n')
        f.write('%}\n\n')
        f.write('%union {\n')
        f.write('    Node *node;\n')
        f.write('}\n\n')

        # Collect token names from lex rules (skip WHITESPACE)
        tokens = set()
        for rule in lex_rules:
            if rule.token_name != 'WHITESPACE':
                tokens.add(rule.token_name)

        # Emit %token declarations with the Node* semantic type
        for token in sorted(tokens):
            f.write('%token <node> ' + token + '\n')
        f.write('\n')

        # Declare %type for each nonterminal
        nonterminals = set()
        for rule in grammar_rules:
            nonterminals.add(rule.lhs)

        for nt in sorted(nonterminals):
            f.write('%type <node> ' + nt + '\n')
        f.write('\n')

        # Set the start symbol to the first declared LHS
        if grammar_rules:
            f.write('%start ' + grammar_rules[0].lhs + '\n\n')

        f.write('%%\n\n')

        # Emit grammar rules. The generator writes rules grouped by LHS
        # using '|' for alternatives and preserves any user-provided actions.
        current_lhs = None
        is_first_rule = True
        for rule in grammar_rules:
            if rule.lhs != current_lhs:
                if current_lhs is not None:
                    f.write('    ;\n\n')
                f.write(rule.lhs + ':\n')
                current_lhs = rule.lhs
                first = True
            else:
                f.write('    | ')
                first = False

            if first:
                f.write('    ')

            f.write(rule.rhs if rule.rhs else '/* empty */')

            # Attach action code if present. For the very first grammar rule
            # record the root AST node in `ast_root`.
            if rule.action:
                f.write('\n        { ' + rule.action)
                if is_first_rule:
                    f.write(' ast_root = $$;')
                f.write(' }')
            elif is_first_rule:
                f.write('\n        { ast_root = $$; }')

            f.write('\n')
            is_first_rule = False

        if current_lhs is not None:
            f.write('    ;\n\n')

        # Epilogue: error handler and a small main() that runs the parser
        f.write('%%\n\n')
        f.write('void yyerror(const char *s) {\n')
        f.write('    fprintf(stderr, "Parse error: %s\\n", s);\n')
        f.write('}\n\n')
        f.write('int main(int argc, char **argv) {\n')
        f.write('    if (argc > 1) {\n')
        f.write('        FILE *file = fopen(argv[1], "r");\n')
        f.write('        if (!file) {\n')
        f.write('            perror(argv[1]);\n')
        f.write('            return 1;\n')
        f.write('        }\n')
        f.write('        yyin = file;\n')
        f.write('    }\n\n')
        f.write('    int result = yyparse();\n\n')
        f.write('    if (result == 0 && ast_root != NULL) {\n')
        f.write('        printf("\\n=== Parse Tree ===\\n");\n')
        f.write('        print_ast(ast_root, 0);\n')
        f.write('        free_ast(ast_root);\n')
        f.write('    }\n\n')
        f.write('    return result;\n')
        f.write('}\n')

def generate_token_files(lex_rules: List[LexRule], def_file: str):
    """Generate token example files for the analyzer"""
    # Get the directory containing the .def file
    def_path = Path(def_file)
    output_dir = def_path.parent
    
    # Skip tokens that are typically punctuation or whitespace
    skip_tokens = {
        'WHITESPACE', 'NEWLINE', 'SPACE', 'TAB', 
        'COMMA', 'DOT', 'COLON', 'SEMICOLON', 
        'LPAREN', 'RPAREN', 'LBRACE', 'RBRACE',
        'LBRACKET', 'RBRACKET', 'QUOTE', 'DQUOTE',
        'PLUS', 'MINUS', 'TIMES', 'DIVIDE', 'STAR',
        'EQUALS', 'DASH', 'SLASH', 'BACKSLASH',
        'PERCENT', 'DOLLAR', 'AT', 'HASH', 'EXCLAIM',
        'QUESTION', 'AMPERSAND', 'PIPE', 'CARET',
        'TILDE', 'BACKTICK', 'UNDERSCORE',
        'LETTER', 'DIGIT', 'WORD', 'CHAR'
    }
    
    files_created = 0
    
    for rule in lex_rules:
        token_name = rule.token_name
        
        # Skip common punctuation/whitespace tokens
        if token_name.upper() in skip_tokens:
            continue
        
        # Check if we have a template for this token
        if token_name in TOKEN_TEMPLATES or token_name.upper() in TOKEN_TEMPLATES:
            data = TOKEN_TEMPLATES.get(token_name) or TOKEN_TEMPLATES.get(token_name.upper())
        else:
            # No template available
            continue
        
        # Create token file
        filename = f"{token_name.lower()}_tokens.txt"
        filepath = output_dir / filename
        
        with open(filepath, 'w') as f:
            for item in data:
                f.write(f"{item}\n")
        
        files_created += 1
    
    if files_created > 0:
        print(f'Generated {files_created} token example files in {output_dir}')

def main():
    if len(sys.argv) != 2:
        print('Usage: generator.py <input.def>')
        sys.exit(1)
    
    def_file = sys.argv[1]
    
    print(f'Parsing {def_file}...')
    lex_rules, grammar_rules = parse_def_file(def_file)
    
    print(f'Found {len(lex_rules)} lexer rules and {len(grammar_rules)} grammar rules')
    
    print('Generating lexer.l...')
    generate_lexer(lex_rules, 'lexer.l')
    
    print('Generating parser.y...')
    generate_parser(lex_rules, grammar_rules, 'parser.y')
    
    print('Generating token example files...')
    generate_token_files(lex_rules, def_file)
    
    print('Generation complete!')

if __name__ == '__main__':
    main()
