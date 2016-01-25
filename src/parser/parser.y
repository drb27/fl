%{

#include <string>
#include <iostream>
#include <ostream>
#include <parser/ast.h>
#include <parser/action_target.h>

int yylex(void);
extern "C" void yyerror(const char*);

extern action_target* target;

 static std::string add_str("add");
 static std::string dec_str("dec");

%}
%token EQUALITY
%token BAR
%token SELECTOR
%token DEFAULT
%token OPEN_CURLY
%token CLOSE_CURLY
%token TRACE
%token DEBUG
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
%token ALIAS
%token INTEGER
%token STRING
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
%token SEMICOLON

%union
{
    int int_val;
    std::string* string_val;
    ast* node_val;
}

%type <int_val> INTEGER
%type <string_val> SYMBOL
%type <string_val> STRING
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
%type <node_val> str
%type <node_val> attr
%type <node_val> sequence
%type <node_val> selector
%type <node_val> selpair
%start input

%left SEMICOLON
%precedence MAPSTO
%left ALIAS
%left EQ
%left SELECTOR
%left BAR
%left COLON
%left QUESTION
%precedence EQUALITY
%left BUILDER
%left ADD
%left DECREMENT
%precedence LOWEST
%precedence OPEN_PAREN
%precedence DOT
%precedence QUOTE
%%

/* INPUTS *****************************************************************/

input: stmt { YYACCEPT; };

/* CONSTRUCTORS ***********************************************************/

integer: INTEGER { $$=target->make_int($1); }
symbol:  SYMBOL  { $$=target->make_symbol($1);  }
str: STRING { $$=target->make_string($1); }

/* DEFINITIONS ************************************************************/

fundef: list MAPSTO expr { $$ = target->make_fundef($1,$3); };

/* LISTS ******************************************************************/

list_literal: OPEN_PAREN { target->start_list(); } items_empty CLOSE_PAREN { $$=target->finish_list();};

list_symbol: QUOTE symbol { $$=$2; };

list: list_literal | list_symbol;

items_empty: | items;

 items:       expr { target->push_list_element($1); }
     | items COMMA expr { target->push_list_element($3); }
           ;

/* EXPRESSIONS ************************************************************/

expr:   literal
      | if 
      | funcall
      | fundef
      | attr
      | sequence
      | listbuild
      | methodcall
      | alias
      | assign
      | selector
      | symbol %prec LOWEST 
      | expr EQUALITY expr { $$=target->make_equality($1,$3); }
      | expr ADD expr  { auto as = new std::string(add_str); 
	                 $$=target->make_methodcall($1, target->make_symbol(as), 
				       (list_node*)(target->make_single_list($3))); }
      | expr DECREMENT { auto ds = new std::string(dec_str); 
                         $$=target->make_methodcall($1, target->make_symbol(ds),
				       (list_node*)(target->make_empty_list())); }
      ;

literal: null | bool | integer | str | list_literal;
funcall: symbol list %prec OPEN_PAREN { $$=target->make_funcall($1,$2); };

methodcall: expr DOT symbol list %prec OPEN_PAREN {$$=target->make_methodcall($1,$3,(list_node*)$4);}; 
attr: expr DOT SYMBOL {$$=target->make_attr($1,$3); };

bool: TRUE {$$=target->make_bool(true); } 
    | FALSE { $$=target->make_bool(false); }
    ;

null: NULLVAL { $$=target->make_null(); }
 
alias: expr ALIAS expr { $$ = target->make_assign_node($1,$3,true);};

if: expr QUESTION expr COLON expr { $$ = target->make_ifnode($1,$3,$5); };

listbuild: expr BUILDER expr { $$ = target->build_list($1,$3); };

sequence: OPEN_CURLY { $<node_val>$ = target->make_seq(); } exprs CLOSE_CURLY { $$=$<node_val>2; target->finish_seq(); };

exprs: expr { target->add_expr($1); }
     | exprs SEMICOLON expr { target->add_expr($3); }
     ;

selector: expr SELECTOR {$<node_val>$=target->make_selector($1); } selset { $$=$<node_val>3; target->finish_selector(); };
selset: selpair | selset BAR selpair;
selpair: expr COLON expr { target->selector_condition( target->make_pair($1,$3) ); }
       | DEFAULT COLON expr { target->selector_default($3); };

/* COMMANDS ***************************************************************/

command: trace_cmd | debug_cmd | render_cmd | quit_cmd;

render_cmd: RENDER expr {target->render($2); };

debug_cmd: DEBUG expr { target->enable_debug(); target->respond($2); target->enable_debug(false); };

trace_cmd: TRACE expr { target->enable_trace(); target->respond($2); target->enable_trace(false); };

quit_cmd: QUIT { target->done(); };

/* STATEMENTS *************************************************************/

stmt : expr NEWLINE {target->respond($1);}
     | command NEWLINE {}
     | NEWLINE {};

assign: expr EQ expr { $$=target->make_assign_node($1,$3); };
