#!/usr/bin/env python3
"""cfg2yacc generator module.

This script transforms a ``.def`` grammar specification into the collection of
generated artefacts the build expects:

* ``lexer.l`` – Flex rules derived from ``%%LEX``
* ``parser.y`` – Bison grammar synthesised from ``%%YACC``
* ``*_tokens.txt`` – token samples harvested from the analyzer's input file

It is invoked from the ``Makefile`` as part of ``make all`` / ``make run`` and
acts as the bridge between the high-level declarative grammar and the concrete
Flex/Bison sources compiled into ``custom_compiler``.  Every public function
carries a docstring so the file doubles as living documentation for the
generation pipeline.
"""

import sys
import re
from pathlib import Path
from typing import List, Tuple, Optional


class LexRule:
    def __init__(self, token_name: str, regex: str):
        self.token_name = token_name
        self.regex = regex

    # ``LexRule`` mirrors one line from the ``%%LEX`` block. ``token_name`` is
    # the symbolic token the parser sees while ``regex`` is the original
    # Flex-compatible pattern copied verbatim into ``lexer.l``.

class GrammarRule:
    def __init__(self, lhs: str, rhs: str, action: str):
        self.lhs = lhs
        self.rhs = rhs
        self.action = action

    # ``GrammarRule`` represents a single production taken from ``%%YACC``. The
    # left-hand side non-terminal is stored in ``lhs``, the textual production
    # in ``rhs`` and any associated semantic action (from ``{ ... }``) in
    # ``action``.

def parse_def_file(filename: str) -> Tuple[List[LexRule], List[GrammarRule]]:
    """Parse a ``.def`` analyzer file into lexer and grammar structures.

    Expects exactly one ``%%LEX`` followed by one ``%%YACC`` section.  The
    approach is intentionally pragmatic: it scans lines, extracts token /
    regex pairs, and records grammar productions including simple semantic
    actions.  Extremely elaborate multi-line actions may require manual touch
    ups, but typical samples are supported.
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


def flex_regex_to_python(pattern: str) -> str:
    """Convert a Flex regex into a Python-compatible pattern string."""

    converted = pattern.strip()
    # Flex uses escaped forward slashes for literal '/', translate to Python style.
    converted = converted.replace('\/', '/')
    converted = converted.replace('\\"', '"')

    # Surround bare patterns with a non-capturing group to avoid precedence issues
    if converted and not converted.startswith('('):
        converted = f'(?:{converted})'

    return converted


def find_input_file(def_path: Path) -> Optional[Path]:
    """Infer the most appropriate sample input file for an analyzer."""

    # Common convention: `S1_analyzer.def` -> `S1_input.txt`
    base_name = def_path.stem
    candidates = []

    if base_name.endswith('_analyzer'):
        candidates.append(def_path.with_name(base_name.replace('_analyzer', '_input') + '.txt'))

    # Same-name .txt fallback (e.g., my_grammar.def -> my_grammar.txt)
    candidates.append(def_path.with_suffix('.txt'))

    # Search sibling files that contain 'input' in their stem for best effort
    for candidate in def_path.parent.glob('*'):
        if candidate.is_file() and candidate.suffix.lower() == '.txt' and 'input' in candidate.stem.lower():
            candidates.append(candidate)

    for candidate in candidates:
        if candidate.exists():
            return candidate

    return None

def generate_lexer(lex_rules: List[LexRule], output_file: str):
    """Emit a Flex ``lexer.l`` implementation from parsed ``LexRule`` entries."""

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

        # Emit each lexer rule. The ``.regex`` is written verbatim; the .def
        # author is responsible for providing Flex-compatible patterns.
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

        # Fallback rule for unexpected input characters
        f.write('\n.    { fprintf(stderr, "Unexpected character: %s\\n", yytext); }\n')
        f.write('%%\n\n')
        f.write('int yywrap() { return 1; }\n')

def generate_parser(lex_rules: List[LexRule], grammar_rules: List[GrammarRule], output_file: str):
    """Produce a Bison ``parser.y`` using the collected rule definitions."""

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

        # Gather token names from lex rules (skip WHITESPACE)
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

        # Emit grammar rules grouped by LHS, using '|' for alternatives and
        # preserving any user-provided actions.
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
            # record the root AST node in ``ast_root``.
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
    """
    Generate token example files by scanning the analyzer's sample input.

     Steps performed:
     1. Locate the input file associated with the `.def` analyzer.
     2. Remove any previously generated `*_tokens.txt` files so that output is
         always derived from the latest input.
     3. For each lex rule (excluding punctuation/whitespace tokens), compile a
         Python regex equivalent of the Flex pattern and find unique matches in the
         input text.
     4. Write the matches to `{token_name}_tokens.txt` in the analyzer directory.
    """

    def_path = Path(def_file)
    output_dir = def_path.parent

    input_file = find_input_file(def_path)
    if input_file is None or not input_file.exists():
        print(f'No input file found for {def_path.name}; skipping token generation.')
        return

    try:
        text = input_file.read_text()
    except OSError as exc:
        print(f'Could not read input file {input_file}: {exc}')
        return

    # Remove previously generated token files in this analyzer directory
    removed = 0
    for token_file in output_dir.glob('*_tokens.txt'):
        try:
            token_file.unlink()
            removed += 1
        except OSError as exc:
            print(f'Warning: could not remove {token_file.name}: {exc}')

    if removed:
        print(f'Removed {removed} old token file(s) from {output_dir}')

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

    max_samples = 50
    files_created = 0

    for rule in lex_rules:
        token_name = rule.token_name

        if token_name.upper() in skip_tokens:
            continue

        pattern = flex_regex_to_python(rule.regex)
        try:
            regex = re.compile(pattern, re.MULTILINE)
        except re.error as exc:
            print(f'Warning: could not compile regex for token {token_name}: {exc}')
            continue

        matches = []
        seen = set()
        for match in regex.finditer(text):
            value = match.group(0)
            if not value:
                continue
            if value in seen:
                continue
            matches.append(value)
            seen.add(value)
            if len(matches) >= max_samples:
                break

        if not matches:
            continue

        filename = f"{token_name.lower()}_tokens.txt"
        filepath = output_dir / filename
        try:
            with open(filepath, 'w') as f:
                for value in matches:
                    f.write(f"{value}\n")
        except OSError as exc:
            print(f'Warning: could not write {filepath.name}: {exc}')
            continue

        files_created += 1

    if files_created > 0:
        print(f'Generated {files_created} token example file(s) in {output_dir} from {input_file.name}')
    else:
        print(f'No token data generated from {input_file.name}')

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
