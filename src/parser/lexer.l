%{

#include "parser.tab.h"

%}

DIGIT [0-9]
WS    [ \t]+

%%

{DIGIT}+ { yylval.int_val = atoi(yytext); return INTEGER; }
{WS}     

%%
