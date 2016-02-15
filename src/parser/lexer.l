%option noyywrap

%{
class object;
class ftlist;

#include <stdlib.h>
#include <string>
#include <list>
#include <inc/common.h>
#include <parser/ast/ast.h>
#include <parser/bison.h>

#define STR(x) new std::string(x)
#define FLT(x) atof(x)

%}

%option reentrant
%option bison-bridge

DIGIT [0-9]
WS    [ \t]+
LETTER [A-Za-z_]
BANANA (-{0,1}{DIGIT}+[.]{DIGIT}+)

%%

{BANANA} { yylval->float_val = FLT(yytext); return FLOAT; }
<<EOF>>  { return EOFF; }
while    { return WHILE; }
defenum  { return ENUM; }
defclass { return CLASS; }
\[       { return OPEN_SQUARE; }
\]       { return CLOSE_SQUARE; }
==       { return EQUALITY; }
\?\?     { return SELECTOR; }
\|       { return BAR; }
default  { return DEFAULT; }
\;       { return SEMICOLON; }
\{       { return OPEN_CURLY; }
\}       { return CLOSE_CURLY; }
\"[^\"]*\" { yylval->string_val =STR(yytext); return STRING; } 
trace    { return TRACE; }
debug    { return DEBUG; }
show     { return SHOW; }
::       { return BUILDER; }
render   { return RENDER; }
quit     { return QUIT; }
include  { return INCLUDE; }
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
-{0,1}{DIGIT}+            { yylval->int_val = atoi(yytext); return INTEGER; }
{WS}     
{LETTER}+ { yylval->string_val = STR(yytext); return SYMBOL; }
\/\/[^\n]*
=        { return EQ; }
\n

%%
