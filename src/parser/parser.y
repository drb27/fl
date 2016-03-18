%{

#include <string>
#include <iostream>
#include <ostream>
#include <inc/references.h>
#include <parser/ast/ast.h>
#include <parser/action_target.h>
#include <parser/ast/selector.h>

int yylex(void);
extern "C" void yyerror(const char*);

extern action_target* target;

 static std::string add_str("add");
 static std::string dec_str("dec");

#define _r(x) astref(x)

%}

%define api.pure full
%define api.push-pull push

%token RANGE
%token NODECOUNT
%token LAZY
%token RAISE
%token OBSERVE
%token HANDLE
%token PKG
%token SCOPE
%token CLASS
%token SHOW
%token CD_CMD
%token FLOAT
%token WHILE
%token EOFF
%token OPEN_SQUARE
%token CLOSE_SQUARE
%token EQUALITY
%token BAR
%token SELECTOR
%token DEFAULT
%token PREDICATE
%token OPEN_CURLY
%token CLOSE_CURLY
%token TRACE
%token DEBUG_CMD
%token EVAL
%token BUILDER
%token RENDER
%token QUIT
%token INCLUDE
%token DECREMENT
%token QUESTION
%token COLON
%token QUOTE
%token DOT
%token ADD
%token MAPSTO
%token ALIAS
%token INTEGER
%token STRING
%token NEWLINE
%token IDENTIFIER
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
%token SEMICOLON
%token ENUM

%union
{
    int int_val;
    double float_val;
    std::string* string_val;
    ast* node_val;
}

%type <node_val> lazy_expr
%type <node_val> range
%type <int_val> INTEGER
%type <float_val> FLOAT
%type <string_val> IDENTIFIER
%type <string_val> STRING
%type <node_val> integer
%type <node_val> flfloat
%type <node_val> if
%type <node_val> raise
%type <node_val> null
%type <node_val> symbol
%type <node_val> symbol_wip
%type <node_val> fundef
%type <node_val> expr
%type <node_val> observed_expr
%type <node_val> enumdef
%type <node_val> classdef
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
%type <node_val> str
%type <node_val> attr
%type <node_val> sequence
%type <node_val> selector
%type <node_val> selpair
%type <node_val> index
%type <node_val> flwhile
%start input

%left SEMICOLON
%precedence MAPSTO
%right RAISE
%right OBSERVE
%right HANDLE
%left EQ
%left COLON
%left SELECTOR
%left BAR
%left QUESTION
%right ALIAS
%left RANGE
%precedence EQUALITY
%right OPEN_CURLY
%left CLOSE_CURLY
%left ADD
%left DECREMENT
%precedence LOWEST
%precedence OPEN_PAREN
%precedence DOT
%left BUILDER
%left SCOPE
%right QUOTE
%left OPEN_SQUARE
%left CLOSE_SQUARE
%right WHILE
%right LAZY
%left PREDICATE
%%


/* INPUTS *****************************************************************/

input: stmt { YYACCEPT; };

/* CONSTRUCTORS ***********************************************************/

integer: INTEGER { $$=target->make_int($1); }
flfloat: FLOAT { $$=target->make_float($1); }
symbol_wip:  IDENTIFIER  { target->push_symbol_identifier($1);  }
           | IDENTIFIER  { target->push_symbol_identifier($1); } SCOPE symbol_wip {$$=$4;};

symbol: symbol_wip { $$ = target->finish_symbol(); };

str: STRING { $$=target->make_string($1); }

/* DEFINITIONS ************************************************************/

fundef: list MAPSTO expr { $$ = target->make_fundef(_r($1),_r($3)); };

/* LISTS ******************************************************************/

list: list_literal | list_symbol;

list_literal: OPEN_PAREN { target->start_list(); } items_empty CLOSE_PAREN { $$=target->finish_list();};

list_symbol: QUOTE symbol { $$=$2; };

items_empty: | items;

items:       expr { target->push_list_element(_r($1)); }
      | expr COLON symbol { target->push_list_element_with_typehint(_r($1),_r($3)); }
      | items COMMA expr { target->push_list_element(_r($3)); }
      | items COMMA expr COLON symbol { target->push_list_element_with_typehint(_r($3),_r($5)); }
      ;

/* EXPRESSIONS ************************************************************/

expr:   literal
      | lazy_expr
      | if
      | observed_expr
      | raise
      | funcall
      | fundef
      | attr
      | index
      | sequence
      | listbuild
      | methodcall
      | enumdef
      | classdef
      | alias
      | range
      | assign
      | selector
      | flwhile
      | symbol %prec LOWEST 
      | expr EQUALITY expr { $$=target->make_equality(_r($1),_r($3)); }
      | expr ADD expr  { auto as = new std::string(add_str); 
                         $$=target->make_methodcall(_r($1), _r(target->make_symbol(as)), 
						    _r(target->make_single_list(_r($3)))); }
      | expr DECREMENT { auto ds = new std::string(dec_str); 
                         $$=target->make_methodcall(_r($1), _r(target->make_symbol(ds)),
				       _r(target->make_empty_list())); }
      ;

literal: null | bool | integer | flfloat | str | list_literal;
funcall: symbol list %prec OPEN_PAREN { $$=target->make_funcall(_r($1),_r($2)); };

lazy_expr: LAZY expr { $$=target->make_lazy(_r($2)); }

methodcall: expr DOT symbol list %prec OPEN_PAREN {$$=target->make_methodcall(_r($1),_r($3),_r($4));}; 
attr: expr DOT IDENTIFIER {$$=target->make_attr(_r($1),$3); };

bool: TRUE {$$=target->make_bool(true); } 
    | FALSE { $$=target->make_bool(false); }
    ;

null: NULLVAL { $$=target->make_null(); }
 
alias: expr ALIAS expr { $$ = target->make_assign_node(_r($1),_r($3),true);};

if: expr QUESTION expr COLON expr { $$ = target->make_ifnode(_r($1),_r($3),_r($5)); };

listbuild: expr BUILDER expr { $$ = target->build_list(_r($1),_r($3)); };

sequence: OPEN_CURLY { $<node_val>$ = target->make_seq(); } exprs CLOSE_CURLY { $$=$<node_val>2; target->finish_seq(); };

exprs: expr {target->add_expr(_r($1)); } | sexpr;

sexpr: expr SEMICOLON { target->add_expr(_r($1)); }
     | sexpr expr SEMICOLON { target->add_expr(_r($2)); };

selector: expr SELECTOR {$<node_val>$=target->make_selector(_r($1)); } selpred selset { $$=$<node_val>3; target->finish_selector(); };

selpred: 
      | PREDICATE OPEN_CURLY expr CLOSE_CURLY { target->selector_predicate(_r($3)); };

selset: selpair | selset BAR selpair;

selpair: expr COLON expr { target->selector_condition( _r(target->make_pair(_r($1),_r($3))) ); }
       | DEFAULT COLON expr { target->selector_default(_r($3)); };

index: expr OPEN_SQUARE expr CLOSE_SQUARE { $$=target->make_index(_r($1),_r($3)); }

enumdef: ENUM IDENTIFIER list { $$=target->make_enum_class($2,_r($3)); }

classdef: CLASS IDENTIFIER list { $$=target->make_new_class($2,_r($3)); }

flwhile: WHILE OPEN_CURLY expr CLOSE_CURLY expr { $$=target->make_while(_r($3),_r($5)); }

observed_expr: OBSERVE expr HANDLE { target->start_observed_expression(); }
               selset { $$=$2; 
		        target->selector_handle_predicate();
		        $$->set_observer( selectorref(target->finish_selector())) ; }

raise: RAISE expr { $$=target->make_raise(_r($2)); }

range: expr RANGE expr { $$=target->make_range(_r($1),_r($3)); }

/* COMMANDS ***************************************************************/

command: trace_cmd | debug_cmd | render_cmd | quit_cmd | show_cmd | include_cmd | eval_cmd 
       | cd_cmd | pkg_cmd | nodecount_cmd;

render_cmd: RENDER expr {target->render(_r($2)); };

debug_cmd: DEBUG_CMD expr { target->enable_debug(); target->respond(_r($2)); target->enable_debug(false); };

trace_cmd: TRACE expr { target->enable_trace(); target->respond(_r($2)); target->enable_trace(false); };

quit_cmd: QUIT { target->done(); };

show_cmd: SHOW expr { target->show_cmd(_r($2)); };

include_cmd: INCLUDE expr { target->include_cmd(_r($2)); };

eval_cmd: EVAL expr { target->eval_cmd(_r($2)); };

cd_cmd: CD_CMD expr { target->cd_cmd(_r($2)); };

pkg_cmd: PKG symbol { target->switch_package(_r($2)); };

nodecount_cmd: NODECOUNT { target->nodecount_cmd(); };

/* STATEMENTS *************************************************************/

stmt : expr SEMICOLON {target->respond(_r($1)); YYACCEPT; }
| command SEMICOLON {};
//     | {};

assign: expr EQ expr { $$=target->make_assign_node(_r($1),_r($3)); };
