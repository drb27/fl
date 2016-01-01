%{

#include <string>
#include <parser/ast.h>
#include <parser/action_target.h>

int yylex(void);
extern "C" void yyerror(const char*);

extern action_target* target;

%}

%token ADD
%token MAPSTO
%token INTEGER
%token NEWLINE
%token DIV
%token SYMBOL
%token EQ
%token OPEN_PAREN
%token CLOSE_PAREN
%token OPEN_ANGLED
%token CLOSE_ANGLED
%token COMMA

%union
{
    int int_val;
    std::string* string_val;
    ast* node_val;
}

%type <int_val> INTEGER
%type <node_val> integer
%type <string_val> SYMBOL
%start input

%%

input: /* empty */
     | stmts;

list: OPEN_PAREN items_empty CLOSE_PAREN;

items_empty: | items;

items: literal | items literal;

literal: integer | SYMBOL | list | tuple;

expr: literal | funcall | expr ADD expr;

stmt : assign NEWLINE
     | fundef NEWLINE
     | expr NEWLINE
     ;

stmts: stmt | stmts stmt;

fundef: SYMBOL list MAPSTO expr;

assign: typespec SYMBOL EQ integer { /*$$=target->assign($1,$2,$4);*/ };
integer: INTEGER { /* $$=target->make_int($1); */ }


typespec: SYMBOL | SYMBOL OPEN_ANGLED typespeclist CLOSE_ANGLED;

typespeclist: typespec 
            | typespeclist COMMA typespec;


funcall: expr tuple;

tuple: OPEN_PAREN tupleitems CLOSE_PAREN;

tupleitems: literalz literal;

literalz: literal COMMA | literalz literal COMMA;
