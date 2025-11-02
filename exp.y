%{
#include "ast.h"
#include <stdio.h>
typedef void* YYSTYPE;
extern int yylex(YYSTYPE *);
extern int yyparse(void);
void yyerror(const char *msg);
%}

%define api.pure full
%define parse.error verbose
%define api.value.type {void*}

%token NUMBER

%start expr

%%
expr__gen_1:
    /* empty */ { $$ = NULL; }
  | expr__gen_1__gen_2 expr__gen_1 { $$ = NULL; }
  ;

expr__gen_1__gen_2__gen_3:
    '+' { $$ = NULL; }
  | '-' { $$ = NULL; }
  ;

term:
    factor term__gen_4 { $$ = NULL; }
  ;

expr__gen_1__gen_2:
    '+' term { $$ = NULL; }
  | '-' term { $$ = NULL; }
  ;

expr:
    factor term__gen_4 expr__gen_1 { $$ = NULL; }
  ;

term__gen_4:
    /* empty */ { $$ = NULL; }
  | term__gen_4__gen_5 term__gen_4 { $$ = NULL; }
  ;

term__gen_4__gen_5__gen_6:
    '*' { $$ = NULL; }
  | '/' { $$ = NULL; }
  ;

factor:
    NUMBER { $$ = NULL; }
  | '(' expr ')' { $$ = NULL; }
  ;

term__gen_4__gen_5:
    '*' factor { $$ = NULL; }
  | '/' factor { $$ = NULL; }
  ;

%%

void yyerror(const char *s) { fprintf(stderr, "parse error: %s\n", s); }
int main(void) { return yyparse(); }
