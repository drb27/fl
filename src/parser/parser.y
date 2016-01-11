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
%token TRUE
%token FALSE
%token NULLVAL

%union
{
    int int_val;
    std::string* string_val;
    ast* node_val;
}

%type <int_val> INTEGER
%type <string_val> SYMBOL
%type <node_val> integer
%type <node_val> null
%type <node_val> symbol
%type <node_val> fundef
%type <node_val> expr
%type <node_val> alias
%type <node_val> funcall
%type <node_val> methodcall
%type <node_val> literal
%type <node_val> list
%type <node_val> stmt
%type <node_val> assign
%type <node_val> bool

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

 list:        OPEN_PAREN { target->start_list(); } items_empty CLOSE_PAREN { $$=target->finish_list();};
items_empty: | items;

 items:       expr { target->push_list_element($1); }
     | items expr { target->push_list_element($2); }
           ;

/* EXPRESSIONS ************************************************************/

expr:   literal 
      | funcall
      | methodcall
      | alias
      | assign
      | symbol %prec LOWEST 
      | expr ADD expr
      ;

literal: null | bool | integer | list;
funcall: symbol list %prec OPEN_PAREN;

methodcall: expr DOT SYMBOL list %prec OPEN_PAREN {$$=target->make_methodcall($1,$3,(list_node*)$4);}; 

bool: TRUE {$$=target->make_bool(true); } | FALSE { $$=target->make_bool(false); };

null: NULLVAL { $$=target->make_null(); }
 
alias: symbol MAPSTO symbol { $$ = target->make_alias($1,$3);};

/* STATEMENTS *************************************************************/

stmt : 
       fundef NEWLINE {$$=$1; }
     | expr NEWLINE {$$=$1;}
     ;

stmts: stmt {target->respond($1); } | stmts stmt {target->respond($2); };

 assign: symbol EQ expr { $$=target->make_assign_node($1,$3); };
