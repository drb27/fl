%{

#include <string>
#include "actions.h"

int yylex(void);
extern "C" void yyerror(const char*);

%}

%token INTEGER
%token NEWLINE
%token DIV
%token SYMBOL
%token EQ

%union
{
    int int_val;
    std::string* string_val;
}

%type <int_val> INTEGER
%type <string_val> SYMBOL
%start input

%%

input: /* empty */
     | stmts;

stmt : integer NEWLINE
     | assign NEWLINE
     | SYMBOL NEWLINE { ac_show_def($1); }
     | fraction NEWLINE;

stmts: stmt | stmts stmt;

assign: SYMBOL EQ INTEGER { ac_assign($1,$3); }

integer: INTEGER { ac_integer($1); };
fraction: INTEGER DIV INTEGER { ac_fraction($1,$3); };
