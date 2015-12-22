%{

#include <string>
#include <list>
#include "actions.h"

int yylex(void);
extern "C" void yyerror(const char*);
 class object;
 class ftlist;
%}

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
    object* obj_ptr;
    ftlist* lst_ptr;
    std::list<object*>* obj_list;
}

%type <int_val> INTEGER
%type <string_val> SYMBOL
%type <obj_list> list;
%start input

%%

input: /* empty */
     | stmts;

list: OPEN_PAREN items_empty CLOSE_PAREN {$$=$<obj_list>2;};

items_empty: | items;

items: literal | items literal;

literal: integer | SYMBOL | list | tuple;

expr: literal | funcall;

stmt : assign NEWLINE
     | expr NEWLINE
     ;

stmts: stmt | stmts stmt;

assign: typespec SYMBOL EQ INTEGER { ac_assign($2,$4); }
integer: INTEGER { ac_integer($1); };

typespec: SYMBOL | SYMBOL OPEN_ANGLED typespeclist CLOSE_ANGLED;

funcall: expr tuple;

typespeclist: typespec 
            | typespeclist COMMA typespec;

tuple: OPEN_PAREN tupleitems CLOSE_PAREN;

tupleitems: literalz literal;

literalz: literal COMMA | literalz literal COMMA;
