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

%token BUILDER
%token RENDER
%token QUIT
%token DECREMENT
%token QUESTION
%token COLON
%token QUOTE
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
%type <node_val> if
%type <node_val> null
%type <node_val> symbol
%type <node_val> fundef
%type <node_val> expr
%type <node_val> alias
%type <node_val> funcall
%type <node_val> methodcall
%type <node_val> literal
%type <node_val> list
%type <node_val> list_literal
%type <node_val> list_symbol
%type <node_val> listbuild
%type <node_val> assign
%type <node_val> bool
%start input

%precedence MAPSTO
%precedence QUESTION
%precedence COLON
%left EQ
%left BUILDER
%left ADD
%left DECREMENT
%precedence LOWEST
%precedence OPEN_PAREN
%precedence DOT
%precedence QUOTE
%%


/* INPUTS *****************************************************************/

input: /* empty */
     | stmts;


/* CONSTRUCTORS ***********************************************************/

integer: INTEGER { $$=target->make_int($1); }
symbol:  SYMBOL  { $$=target->make_symbol($1);  }


/* DEFINITIONS ************************************************************/

fundef: list MAPSTO expr { $$ = target->make_fundef($1,$3); };

/*
typealias: TYPEDEF symbol typespec;

typespec: symbol | symbol OPEN_ANGLED typespeclist CLOSE_ANGLED;

typespeclist: typespec 
            | typespeclist COMMA typespec;
*/

/* LISTS ******************************************************************/

list_literal:        OPEN_PAREN { target->start_list(); } items_empty CLOSE_PAREN { $$=target->finish_list();};

list_symbol: QUOTE symbol { $$=$2; };

list: list_literal | list_symbol;

items_empty: | items;

 items:       expr { target->push_list_element($1); }
     | items expr { target->push_list_element($2); }
           ;

/* EXPRESSIONS ************************************************************/

expr:   literal
      | if 
      | funcall
      | fundef
      | listbuild
      | methodcall
      | alias
      | assign
      | symbol %prec LOWEST 
      | expr ADD expr { $$=target->make_methodcall($1, new std::string("add"),
						   (list_node*)(target->make_single_list($3))); }
      | expr DECREMENT { $$=target->make_methodcall($1,new std::string("dec"),
						    (list_node*)(target->make_empty_list())); }
      ;

literal: null | bool | integer | list_literal;
funcall: symbol list %prec OPEN_PAREN { $$=target->make_funcall($1,$2); };

methodcall: expr DOT SYMBOL list %prec OPEN_PAREN {$$=target->make_methodcall($1,$3,(list_node*)$4);}; 

bool: TRUE {$$=target->make_bool(true); } | FALSE { $$=target->make_bool(false); };

null: NULLVAL { $$=target->make_null(); }
 
alias: symbol MAPSTO symbol { $$ = target->make_alias($1,$3);};

if: expr QUESTION expr COLON expr { $$ = target->make_ifnode($1,$3,$5); };

listbuild: expr BUILDER expr { $$ = target->build_list($1,$3); };

/* COMMANDS ***************************************************************/

command: render_cmd | quit_cmd;

render_cmd: RENDER expr {target->render($2); };

quit_cmd: QUIT { YYACCEPT; };

/* STATEMENTS *************************************************************/

stmt : expr NEWLINE {target->respond($1);}
     | command NEWLINE {};

stmts: stmt | stmts stmt;

 assign: symbol EQ expr { $$=target->make_assign_node($1,$3); };
