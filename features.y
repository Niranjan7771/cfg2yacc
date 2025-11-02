%{
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
extern int yylex(void);
extern int yyparse(void);
void yyerror(const char *msg);
%}

%define parse.error verbose

%token ID NUMBER

%start list

%%
list__gen_1:
    /* empty */ { printf("  list__gen_1 -> ε\n"); $$ = 0; }
  | items { printf("  list__gen_1 -> items\n"); $$ = 0; }
  ;

items:
    ID items__gen_2 { printf("  items -> ID items__gen_2\n"); $$ = 0; }
  ;

list:
    '(' list__gen_1 ')' { printf("  list -> '(' list__gen_1 ')'\n"); $$ = 0; }
  | item list__gen_7 { printf("  list -> item list__gen_7\n"); $$ = 0; }
  ;

items__gen_2:
    /* empty */ { printf("  items__gen_2 -> ε\n"); $$ = 0; }
  | items__gen_2__gen_3 items__gen_2 { printf("  items__gen_2 -> items__gen_2__gen_3 items__gen_2\n"); $$ = 0; }
  ;

items__gen_2__gen_3:
    ',' ID { printf("  items__gen_2__gen_3 -> ',' ID\n"); $$ = 0; }
  ;

repeat__gen_4__gen_5:
    /* empty */ { printf("  repeat__gen_4__gen_5 -> ε\n"); $$ = 0; }
  | ID repeat__gen_4__gen_5 { printf("  repeat__gen_4__gen_5 -> ID repeat__gen_4__gen_5\n"); $$ = 0; }
  ;

repeat__gen_4:
    ID repeat__gen_4__gen_5 { printf("  repeat__gen_4 -> ID repeat__gen_4__gen_5\n"); $$ = 0; }
  ;

repeat:
    ID repeat__gen_4__gen_5 { printf("  repeat -> ID repeat__gen_4__gen_5\n"); $$ = 0; }
  ;

maybe__gen_6:
    /* empty */ { printf("  maybe__gen_6 -> ε\n"); $$ = 0; }
  | ID { printf("  maybe__gen_6 -> ID\n"); $$ = 0; }
  ;

maybe:
    /* empty */ { printf("  maybe -> ε\n"); $$ = 0; }
  | ID { printf("  maybe -> ID\n"); $$ = 0; }
  ;

list__gen_7:
    /* empty */ { printf("  list__gen_7 -> ε\n"); $$ = 0; }
  | list__gen_7__gen_8 list__gen_7 { printf("  list__gen_7 -> list__gen_7__gen_8 list__gen_7\n"); $$ = 0; }
  ;

item:
    ID item__gen_9 { printf("  item -> ID item__gen_9\n"); $$ = 0; }
  ;

list__gen_7__gen_8:
    ',' item { printf("  list__gen_7__gen_8 -> ',' item\n"); $$ = 0; }
  ;

item__gen_9:
    /* empty */ { printf("  item__gen_9 -> ε\n"); $$ = 0; }
  | '[' NUMBER ']' { printf("  item__gen_9 -> '[' NUMBER ']'\n"); $$ = 0; }
  ;

opt__gen_10:
    /* empty */ { printf("  opt__gen_10 -> ε\n"); $$ = 0; }
  | '+' { printf("  opt__gen_10 -> '+'\n"); $$ = 0; }
  | '-' { printf("  opt__gen_10 -> '-'\n"); $$ = 0; }
  ;

opt:
    NUMBER { printf("  opt -> NUMBER\n"); $$ = 0; }
  | '+' NUMBER { printf("  opt -> '+' NUMBER\n"); $$ = 0; }
  | '-' NUMBER { printf("  opt -> '-' NUMBER\n"); $$ = 0; }
  ;

rep__gen_11__gen_12:
    /* empty */ { printf("  rep__gen_11__gen_12 -> ε\n"); $$ = 0; }
  | ')' rep__gen_11__gen_12 { printf("  rep__gen_11__gen_12 -> ')' rep__gen_11__gen_12\n"); $$ = 0; }
  ;

rep__gen_11:
    ')' rep__gen_11__gen_12 { printf("  rep__gen_11 -> ')' rep__gen_11__gen_12\n"); $$ = 0; }
  ;

rep:
    '(' ID ID rep__gen_11 { printf("  rep -> '(' ID ID rep__gen_11\n"); $$ = 0; }
  ;

%%

void yyerror(const char *s) { fprintf(stderr, "❌ Parse error: %s\n", s); }
int main(void) {
    printf("🌳 Parse Tree:\n");
    int result = yyparse();
    if (result == 0) {
        printf("✅ Input parsed successfully!\n");
    } else {
        printf("❌ Parse failed!\n");
    }
    return result;
}
