%{
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
extern int yylex(void);
extern int yyparse(void);
void yyerror(const char *msg);
%}

%define parse.error verbose

%token NUMBER

%start expr

%%
expr__gen_1:
    /* empty */ { $$ = 0; }
  | expr__gen_1__gen_2 expr__gen_1 { $$ = 0; }
  ;

expr__gen_1__gen_2__gen_3:
    '+' { $$ = 0; }
  | '-' { $$ = 0; }
  ;

term:
    factor term__gen_4 { $$ = 0; }
  ;

expr__gen_1__gen_2:
    '+' term { $$ = 0; }
  | '-' term { $$ = 0; }
  ;

expr:
    factor term__gen_4 expr__gen_1 { $$ = 0; }
  ;

term__gen_4:
    /* empty */ { $$ = 0; }
  | term__gen_4__gen_5 term__gen_4 { $$ = 0; }
  ;

term__gen_4__gen_5__gen_6:
    '*' { $$ = 0; }
  | '/' { $$ = 0; }
  ;

factor:
    NUMBER { $$ = 0; }
  | '(' expr ')' { $$ = 0; }
  ;

term__gen_4__gen_5:
    '*' factor { $$ = 0; }
  | '/' factor { $$ = 0; }
  ;

%%

void yyerror(const char *s) { fprintf(stderr, "parse error: %s\n", s); }
int main(void) { return yyparse(); }
