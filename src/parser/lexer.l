%option noyywrap

%{
class object;
class ftlist;

#include <string>
#include <list>
#include <common.h>
#include <parser/ast.h>
#include "parser.h"


#define STR(x) new std::string(x)


%}

DIGIT [0-9]
WS    [ \t]+
LETTER [A-Za-z_]
%%

enum     { return ENUM; }
\[       { return OPEN_SQUARE; }
\]       { return CLOSE_SQUARE; }
==       { return EQUALITY; }
\?\?     { return SELECTOR; }
\|       { return BAR; }
default  { return DEFAULT; }
\;       { return SEMICOLON; }
\{       { return OPEN_CURLY; }
\}       { return CLOSE_CURLY; }
\"[^\"]*\" { yylval.string_val =STR(yytext); return STRING; } 
trace    { return TRACE; }
debug    { return DEBUG; }
::       { return BUILDER; }
render   { return RENDER; }
quit     { return QUIT; }
\-\-     { return DECREMENT; }
\?       { return QUESTION; }
\:       { return COLON; }
\'       { return QUOTE; }
null     { return NULLVAL; }
true     { return TRUE; }
false    { return FALSE; }
\.       { return DOT; }
typedef  { return TYPEDEF; }
\+       { return ADD; }
-\>      { return MAPSTO; }
=\>      { return ALIAS; }
,        { return COMMA; }
\(       { return OPEN_PAREN; }
\)       { return CLOSE_PAREN; }
\<       { return OPEN_ANGLED; }
\>       { return CLOSE_ANGLED; }
{DIGIT}+            { yylval.int_val = atoi(yytext); return INTEGER; }
{WS}     
{LETTER}+ { yylval.string_val = STR(yytext); return SYMBOL; }
\/\/[^\n]*
=        { return EQ; }
\n       { return NEWLINE; }

%%
