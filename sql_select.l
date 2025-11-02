%{
#include "sql_select.tab.h"
#include <stdlib.h>
%}

%option noyywrap

%%

\*          return '*';
;           return ';';
\.          return '.';
,           return ',';
[0-9]+(\.[0-9]+)?  return NUMBER;
"\""(\\.|[^"\\])*"\""  return STRING;
"WHERE"       return WHERE;
"ORDER"       return ORDER;
"BY"       return BY;
"SELECT"       return SELECT;
"FROM"       return FROM;
"OR"       return OR;
"AND"       return AND;
"="       return EQ;
"<>"       return NEQ;
"<"       return LT;
"<="       return LEQ;
">"       return GT;
">="       return GEQ;
"ASC"       return ASC;
"DESC"       return DESC;
[A-Za-z_][A-Za-z0-9_]*  return IDENTIFIER;
[ \t\r\n]+  /* skip whitespace */;
.         return yytext[0];

%%

/* User code section - add any additional C code here */
