%option noyywrap

%{
#include <string>
#include <common.h>
#include "parser.h"

#define STR(x) new std::string(x)
%}

DIGIT [0-9]
WS    [ \t]+
LETTER [A-Za-z]
%%


{DIGIT}+            { yylval.int_val = atoi(yytext); return INTEGER; }
{WS}     
{LETTER}+ { yylval.string_val = STR(yytext); return SYMBOL; }
=        { return EQ; }
\/       { return DIV; }
\n       { return NEWLINE; }

%%
