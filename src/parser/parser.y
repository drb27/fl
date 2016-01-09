%{

#include <string>
#include <iostream>
#include <ostream>
#include <parser/ast.h>
#include <parser/action_target.h>

int yylex(void);
extern "C" void yyerror(const char*);

extern action_target* target;

%}

%token DOT
%token ADD
%token MAPSTO
%token INTEGER
%token NEWLINE
%token SYMBOL
%token EQ
%token OPEN_PAREN
%token CLOSE_PAREN
%token OPEN_ANGLED
%token CLOSE_ANGLED
%token COMMA
%token TYPEDEF

%union
{
    int int_val;
    std::string* string_val;
    ast* node_val;
}

%type <int_val> INTEGER
%type <string_val> SYMBOL
%type <node_val> integer
%type <node_val> symbol
%type <node_val> fundef
%type <node_val> expr
%type <node_val> funcall
%type <node_val> methodcall
%type <node_val> literal
%type <node_val> list
%type <node_val> items
%type <node_val> items_empty
%type <node_val> stmt
%type <node_val> assign

%start input

%left ADD
%precedence LOWEST
%precedence OPEN_PAREN
%%


/* INPUTS *****************************************************************/

input: /* empty */
     | stmts;


/* CONSTRUCTORS ***********************************************************/

integer: INTEGER { $$=target->make_int($1); }
symbol:  SYMBOL  { $$=target->make_symbol($1);  }


/* DEFINITIONS ************************************************************/

fundef: symbol list MAPSTO expr { /*$$ = target->make_fundef($1,$4);*/ };

/*
typealias: TYPEDEF symbol typespec;

typespec: symbol | symbol OPEN_ANGLED typespeclist CLOSE_ANGLED;

typespeclist: typespec 
            | typespeclist COMMA typespec;
*/

/* LISTS ******************************************************************/

list:        OPEN_PAREN items_empty CLOSE_PAREN {$$=$2;};
items_empty: {$$=nullptr;} | items;

items:       expr 
           | items expr
           ;

/* EXPRESSIONS ************************************************************/

expr:   literal 
      | funcall
      | methodcall
      | symbol %prec LOWEST 
      | expr ADD expr
      ;

literal: integer | list;
funcall: symbol list %prec OPEN_PAREN;

 methodcall: SYMBOL DOT SYMBOL list %prec OPEN_PAREN {$$=target->make_methodcall($1,$3);}; 

/* STATEMENTS *************************************************************/

stmt : assign NEWLINE {$$=$1; }
     | fundef NEWLINE {$$=$1; }
       /* | typealias NEWLINE */
     | expr NEWLINE {$$=$1;}
     ;

stmts: stmt {target->respond($1); } | stmts stmt {target->respond($2); };

 assign: symbol EQ literal { $$=target->make_assign_node($1,$3); };
