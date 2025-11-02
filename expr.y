%{
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern int yylex(void);
extern int yyparse(void);
void yyerror(const char *msg);
static int parse_step = 0;
static int json_mode = 0;
%}

%define parse.error verbose

%token NUMBER

%start expr

%%
expr__gen_1:
    /* empty */ {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"expr__gen_1 -> ε\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  expr__gen_1 -> ε\n");
        }
        $$ = 0;
    }
  | expr__gen_1__gen_2 expr__gen_1 {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"expr__gen_1 -> expr__gen_1__gen_2 expr__gen_1\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  expr__gen_1 -> expr__gen_1__gen_2 expr__gen_1\n");
        }
        $$ = 0;
    }
  ;

expr__gen_1__gen_2__gen_3:
    '+' {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"expr__gen_1__gen_2__gen_3 -> '+'\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  expr__gen_1__gen_2__gen_3 -> '+'\n");
        }
        $$ = 0;
    }
  | '-' {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"expr__gen_1__gen_2__gen_3 -> '-'\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  expr__gen_1__gen_2__gen_3 -> '-'\n");
        }
        $$ = 0;
    }
  ;

term:
    factor term__gen_4 {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"term -> factor term__gen_4\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  term -> factor term__gen_4\n");
        }
        $$ = 0;
    }
  ;

expr__gen_1__gen_2:
    '+' term {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"expr__gen_1__gen_2 -> '+' term\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  expr__gen_1__gen_2 -> '+' term\n");
        }
        $$ = 0;
    }
  | '-' term {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"expr__gen_1__gen_2 -> '-' term\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  expr__gen_1__gen_2 -> '-' term\n");
        }
        $$ = 0;
    }
  ;

expr:
    factor term__gen_4 expr__gen_1 {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"expr -> factor term__gen_4 expr__gen_1\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  expr -> factor term__gen_4 expr__gen_1\n");
        }
        $$ = 0;
    }
  ;

term__gen_4:
    /* empty */ {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"term__gen_4 -> ε\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  term__gen_4 -> ε\n");
        }
        $$ = 0;
    }
  | term__gen_4__gen_5 term__gen_4 {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"term__gen_4 -> term__gen_4__gen_5 term__gen_4\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  term__gen_4 -> term__gen_4__gen_5 term__gen_4\n");
        }
        $$ = 0;
    }
  ;

term__gen_4__gen_5__gen_6:
    '*' {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"term__gen_4__gen_5__gen_6 -> '*'\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  term__gen_4__gen_5__gen_6 -> '*'\n");
        }
        $$ = 0;
    }
  | '/' {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"term__gen_4__gen_5__gen_6 -> '/'\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  term__gen_4__gen_5__gen_6 -> '/'\n");
        }
        $$ = 0;
    }
  ;

factor:
    NUMBER {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"factor -> NUMBER\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  factor -> NUMBER\n");
        }
        $$ = 0;
    }
  | '(' expr ')' {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"factor -> '(' expr ')'\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  factor -> '(' expr ')'\n");
        }
        $$ = 0;
    }
  ;

term__gen_4__gen_5:
    '*' factor {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"term__gen_4__gen_5 -> '*' factor\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  term__gen_4__gen_5 -> '*' factor\n");
        }
        $$ = 0;
    }
  | '/' factor {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"term__gen_4__gen_5 -> '/' factor\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  term__gen_4__gen_5 -> '/' factor\n");
        }
        $$ = 0;
    }
  ;

%%

void yyerror(const char *s) {
    if (json_mode) {
        printf("{\"error\": \"%s\", \"step\": %d}\n", s, parse_step);
    } else {
        fprintf(stderr, "❌ Parse error: %s\n", s);
    }
}

int main(int argc, char *argv[]) {
    // Check for JSON mode
    if (argc > 1 && strcmp(argv[1], "--json") == 0) {
        json_mode = 1;
        printf("{\"parse_tree\": [");
    } else {
        printf("🌳 Parse Tree:\n");
    }
    
    int result = yyparse();
    
    if (json_mode) {
        printf("], \"success\": %s, \"steps\": %d}\n",
               result == 0 ? "true" : "false", parse_step);
    } else {
        if (result == 0) {
            printf("✅ Input parsed successfully!\n");
        } else {
            printf("❌ Parse failed!\n");
        }
    }
    return result;
}
