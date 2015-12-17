%{

#include "actions.h"

int yylex(void);
void yyerror(const char*);

%}

%token INTEGER
%token NEWLINE

%union
{
    int int_val;
    char* string_val;
}

%type <int_val> INTEGER

%start input

%%

input: /* empty */
     | stmts;

stmt : integer NEWLINE { printf("Statement!\n"); };

stmts: stmt | stmts stmt;

integer: INTEGER { ac_integer($1); };
