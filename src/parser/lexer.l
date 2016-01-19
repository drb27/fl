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
,        { return COMMA; }
\(       { return OPEN_PAREN; }
\)       { return CLOSE_PAREN; }
\<       { return OPEN_ANGLED; }
\>       { return CLOSE_ANGLED; }
{DIGIT}+            { yylval.int_val = atoi(yytext); return INTEGER; }
{WS}     
{LETTER}+ { yylval.string_val = STR(yytext); return SYMBOL; }
=        { return EQ; }
\n       { return NEWLINE; }

%%
