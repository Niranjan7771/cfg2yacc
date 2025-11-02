%{
#include "features.tab.h"
#include <stdlib.h>
%}

%option noyywrap

%%

\(          return '(';
\)          return ')';
,           return ',';
\[          return '[';
\]          return ']';
\+          return '+';
-           return '-';
[0-9]+     return NUMBER;
[A-Za-z_][A-Za-z0-9_]*  return ID;
[ \t\r\n]+  /* skip whitespace */;
.         return yytext[0];

%%

/* User code section - add any additional C code here */
