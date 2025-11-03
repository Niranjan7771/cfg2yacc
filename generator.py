#!/usr/bin/env python3
import sys
from typing import List, Tuple

class LexRule:
    def __init__(self, token_name: str, regex: str):
        self.token_name = token_name
        self.regex = regex

class GrammarRule:
    def __init__(self, lhs: str, rhs: str, action: str):
        self.lhs = lhs
        self.rhs = rhs
        self.action = action

def parse_def_file(filename: str) -> Tuple[List[LexRule], List[GrammarRule]]:
    with open(filename, 'r') as f:
        content = f.read()
    
    if '%%LEX' not in content or '%%YACC' not in content:
        print(f'Error: {filename} must contain both %%LEX and %%YACC sections')
        sys.exit(1)
    
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
    with open(output_file, 'w') as f:
        f.write('%{\n')
        f.write('#include <stdio.h>\n')
        f.write('#include <stdlib.h>\n')
        f.write('#include <string.h>\n')
        f.write('#include "ast.h"\n')
        f.write('#include "y.tab.h"\n')
        f.write('%}\n\n')
        f.write('%%\n\n')
        
        for rule in lex_rules:
            if rule.token_name == 'WHITESPACE':
                f.write(rule.regex + '    { /* skip whitespace */ }\n')
            else:
                f.write(rule.regex + '    { ')
                f.write('yylval.node = create_leaf_node("' + rule.token_name + '", yytext); ')
                f.write('return ' + rule.token_name + '; ')
                f.write('}\n')
        
        f.write('\n.    { fprintf(stderr, "Unexpected character: %s\\n", yytext); }\n')
        f.write('%%\n\n')
        f.write('int yywrap() { return 1; }\n')

def generate_parser(lex_rules: List[LexRule], grammar_rules: List[GrammarRule], output_file: str):
    with open(output_file, 'w') as f:
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
        
        tokens = set()
        for rule in lex_rules:
            if rule.token_name != 'WHITESPACE':
                tokens.add(rule.token_name)
        
        for token in sorted(tokens):
            f.write('%token <node> ' + token + '\n')
        f.write('\n')
        
        nonterminals = set()
        for rule in grammar_rules:
            nonterminals.add(rule.lhs)
        
        for nt in sorted(nonterminals):
            f.write('%type <node> ' + nt + '\n')
        f.write('\n')
        
        if grammar_rules:
            f.write('%start ' + grammar_rules[0].lhs + '\n\n')
        
        f.write('%%\n\n')
        
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
    
    print('Generation complete!')

if __name__ == '__main__':
    main()
