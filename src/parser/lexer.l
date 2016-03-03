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
HEXDIGIT[0-9a-fA-F]
WS    [ \t]+
LETTER [A-Za-z_]
BANANA (-{0,1}{DIGIT}+[.]{DIGIT}+)

%%

nodecount { return NODECOUNT; }
\$       { return LAZY; }
raise    { return RAISE; }
observe  { return OBSERVE; }
handle   { return HANDLE; }
package  { return PKG; }
::       { return SCOPE; }
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
predicate: { return PREDICATE; }
default  { return DEFAULT; }
\;       { return SEMICOLON; }
\{       { return OPEN_CURLY; }
\}       { return CLOSE_CURLY; }
\"[^\"]*\" { yylval->string_val =STR(yytext); return STRING; } 
trace    { return TRACE; }
eval     { return EVAL; }
debug    { return DEBUG_CMD; }
show     { return SHOW; }
cd       { return CD_CMD; }
@        { return BUILDER; }
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
-{0,1}0x{HEXDIGIT}+ { yylval->int_val = strtol(yytext,NULL,16); return INTEGER; }
#.*
{WS}     
{LETTER}({LETTER}|{DIGIT})* { yylval->string_val = STR(yytext); return IDENTIFIER; }
\/\/[^\n]*
=        { return EQ; }
\n

%%
