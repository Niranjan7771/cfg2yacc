%{
#include "y.tab.h"
#include <stdlib.h>
%}

%option noyywrap
%option reentrant

[+]    return '+';
-    return '-';
[*]    return '*';
/    return '/';
NUMBER    return NUMBER;
[(]    return '(';
[)]    return ')';
[0-9]+    return NUMBER;
[\t\r\n ]+    ;
.          return yytext[0];

%%
