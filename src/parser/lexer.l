%option noyywrap

%{

#include "parser.h"

%}

DIGIT [0-9]
WS    [ \t]+

%%

{DIGIT}+ { yylval.int_val = atoi(yytext); return INTEGER; }
{WS}     
\n       { return NEWLINE; }

%%
