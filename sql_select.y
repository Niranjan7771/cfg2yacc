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

%token WHERE ORDER BY SELECT FROM IDENTIFIER OR AND NUMBER STRING EQ NEQ LT LEQ GT GEQ ASC DESC

%start query

%%
query__gen_1:
    /* empty */ {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"query__gen_1 -> ε\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  query__gen_1 -> ε\n");
        }
        $$ = 0;
    }
  | where_clause {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"query__gen_1 -> where_clause\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  query__gen_1 -> where_clause\n");
        }
        $$ = 0;
    }
  ;

where_clause:
    WHERE condition {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"where_clause -> WHERE condition\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  where_clause -> WHERE condition\n");
        }
        $$ = 0;
    }
  ;

query__gen_2:
    /* empty */ {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"query__gen_2 -> ε\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  query__gen_2 -> ε\n");
        }
        $$ = 0;
    }
  | order_clause {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"query__gen_2 -> order_clause\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  query__gen_2 -> order_clause\n");
        }
        $$ = 0;
    }
  ;

order_clause:
    ORDER BY order_term order_clause__gen_9 {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"order_clause -> ORDER BY order_term order_clause__gen_9\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  order_clause -> ORDER BY order_term order_clause__gen_9\n");
        }
        $$ = 0;
    }
  ;

select_list:
    '*' {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"select_list -> '*'\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  select_list -> '*'\n");
        }
        $$ = 0;
    }
  | column select_list__gen_3 {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"select_list -> column select_list__gen_3\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  select_list -> column select_list__gen_3\n");
        }
        $$ = 0;
    }
  ;

query:
    SELECT select_list FROM IDENTIFIER query__gen_1 query__gen_2 ';' {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"query -> SELECT select_list FROM IDENTIFIER query__gen_1 query__gen_2 ';'\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  query -> SELECT select_list FROM IDENTIFIER query__gen_1 query__gen_2 ';'\n");
        }
        $$ = 0;
    }
  ;

select_list__gen_3:
    /* empty */ {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"select_list__gen_3 -> ε\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  select_list__gen_3 -> ε\n");
        }
        $$ = 0;
    }
  | select_list__gen_3__gen_4 select_list__gen_3 {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"select_list__gen_3 -> select_list__gen_3__gen_4 select_list__gen_3\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  select_list__gen_3 -> select_list__gen_3__gen_4 select_list__gen_3\n");
        }
        $$ = 0;
    }
  ;

column:
    IDENTIFIER {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"column -> IDENTIFIER\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  column -> IDENTIFIER\n");
        }
        $$ = 0;
    }
  | IDENTIFIER '.' IDENTIFIER {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"column -> IDENTIFIER '.' IDENTIFIER\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  column -> IDENTIFIER '.' IDENTIFIER\n");
        }
        $$ = 0;
    }
  ;

select_list__gen_3__gen_4:
    ',' column {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"select_list__gen_3__gen_4 -> ',' column\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  select_list__gen_3__gen_4 -> ',' column\n");
        }
        $$ = 0;
    }
  ;

condition:
    disjunction {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"condition -> disjunction\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  condition -> disjunction\n");
        }
        $$ = 0;
    }
  ;

disjunction:
    conjunction disjunction__gen_5 {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"disjunction -> conjunction disjunction__gen_5\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  disjunction -> conjunction disjunction__gen_5\n");
        }
        $$ = 0;
    }
  ;

disjunction__gen_5:
    /* empty */ {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"disjunction__gen_5 -> ε\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  disjunction__gen_5 -> ε\n");
        }
        $$ = 0;
    }
  | disjunction__gen_5__gen_6 disjunction__gen_5 {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"disjunction__gen_5 -> disjunction__gen_5__gen_6 disjunction__gen_5\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  disjunction__gen_5 -> disjunction__gen_5__gen_6 disjunction__gen_5\n");
        }
        $$ = 0;
    }
  ;

conjunction:
    comparison conjunction__gen_7 {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"conjunction -> comparison conjunction__gen_7\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  conjunction -> comparison conjunction__gen_7\n");
        }
        $$ = 0;
    }
  ;

disjunction__gen_5__gen_6:
    OR conjunction {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"disjunction__gen_5__gen_6 -> OR conjunction\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  disjunction__gen_5__gen_6 -> OR conjunction\n");
        }
        $$ = 0;
    }
  ;

conjunction__gen_7:
    /* empty */ {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"conjunction__gen_7 -> ε\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  conjunction__gen_7 -> ε\n");
        }
        $$ = 0;
    }
  | conjunction__gen_7__gen_8 conjunction__gen_7 {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"conjunction__gen_7 -> conjunction__gen_7__gen_8 conjunction__gen_7\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  conjunction__gen_7 -> conjunction__gen_7__gen_8 conjunction__gen_7\n");
        }
        $$ = 0;
    }
  ;

comparison:
    value comparator value {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"comparison -> value comparator value\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  comparison -> value comparator value\n");
        }
        $$ = 0;
    }
  ;

conjunction__gen_7__gen_8:
    AND comparison {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"conjunction__gen_7__gen_8 -> AND comparison\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  conjunction__gen_7__gen_8 -> AND comparison\n");
        }
        $$ = 0;
    }
  ;

value:
    NUMBER {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"value -> NUMBER\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  value -> NUMBER\n");
        }
        $$ = 0;
    }
  | STRING {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"value -> STRING\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  value -> STRING\n");
        }
        $$ = 0;
    }
  | IDENTIFIER {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"value -> IDENTIFIER\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  value -> IDENTIFIER\n");
        }
        $$ = 0;
    }
  | IDENTIFIER '.' IDENTIFIER {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"value -> IDENTIFIER '.' IDENTIFIER\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  value -> IDENTIFIER '.' IDENTIFIER\n");
        }
        $$ = 0;
    }
  ;

comparator:
    EQ {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"comparator -> EQ\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  comparator -> EQ\n");
        }
        $$ = 0;
    }
  | NEQ {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"comparator -> NEQ\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  comparator -> NEQ\n");
        }
        $$ = 0;
    }
  | LT {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"comparator -> LT\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  comparator -> LT\n");
        }
        $$ = 0;
    }
  | LEQ {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"comparator -> LEQ\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  comparator -> LEQ\n");
        }
        $$ = 0;
    }
  | GT {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"comparator -> GT\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  comparator -> GT\n");
        }
        $$ = 0;
    }
  | GEQ {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"comparator -> GEQ\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  comparator -> GEQ\n");
        }
        $$ = 0;
    }
  ;

order_clause__gen_9:
    /* empty */ {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"order_clause__gen_9 -> ε\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  order_clause__gen_9 -> ε\n");
        }
        $$ = 0;
    }
  | order_clause__gen_9__gen_10 order_clause__gen_9 {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"order_clause__gen_9 -> order_clause__gen_9__gen_10 order_clause__gen_9\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  order_clause__gen_9 -> order_clause__gen_9__gen_10 order_clause__gen_9\n");
        }
        $$ = 0;
    }
  ;

order_term:
    IDENTIFIER order_term__gen_11 {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"order_term -> IDENTIFIER order_term__gen_11\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  order_term -> IDENTIFIER order_term__gen_11\n");
        }
        $$ = 0;
    }
  ;

order_clause__gen_9__gen_10:
    ',' order_term {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"order_clause__gen_9__gen_10 -> ',' order_term\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  order_clause__gen_9__gen_10 -> ',' order_term\n");
        }
        $$ = 0;
    }
  ;

order_term__gen_11:
    /* empty */ {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"order_term__gen_11 -> ε\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  order_term__gen_11 -> ε\n");
        }
        $$ = 0;
    }
  | ASC {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"order_term__gen_11 -> ASC\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  order_term__gen_11 -> ASC\n");
        }
        $$ = 0;
    }
  | DESC {
        if (json_mode) {
            printf("%s{\"step\": %d, \"rule\": \"order_term__gen_11 -> DESC\"}",
                   parse_step > 0 ? ",\n  " : "\n  ", ++parse_step);
        } else {
            printf("  order_term__gen_11 -> DESC\n");
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
