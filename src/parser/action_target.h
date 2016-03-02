#ifndef ACTION_TARGET_H
#define ACTION_TARGET_H

#include <string>
#include <list>
#include <iostream>

class ast;
class list_node;
class selector_node;

class action_target
{
 
 public:

    virtual ast* make_lazy(ast* e)=0;
    virtual ast* make_raise(ast* clsExpr)=0;
    virtual ast* make_int(int x) const=0;
    virtual ast* make_string(std::string* x) const=0;
    virtual ast* make_null() const=0;
    virtual ast* make_fundef( ast* arglist,  ast* def) const=0;
    virtual ast* make_funcall( ast* fn,  ast* args) const=0;
    virtual ast* make_ifnode( ast* condExpr,  ast* trueExpr, ast* falseExpr) const=0;
    virtual ast* make_symbol( std::string* name, const std::list<std::string>& scopespec={}) const=0;
    virtual void switch_package( ast* symbol )=0;
    virtual ast* make_alias(ast* alias, ast* existing) const=0;
    virtual ast* make_assign_node(ast* lvalue, ast* rvalue,bool alias=false)=0;
    virtual ast* make_methodcall( ast* target, ast* method,ast* args)=0;
    virtual ast* make_attr( ast* target, std::string* selector)=0;
    virtual ast* make_pair(ast*,ast*)=0;
    virtual ast* make_selector(ast*)=0;
    virtual ast* start_observed_expression()=0;
    virtual ast* make_equality(ast*,ast*)=0;
    virtual ast* make_index(ast*,ast*)=0;
    virtual ast* make_enum_class(std::string*,ast*)=0;
    virtual ast* make_new_class(std::string*,ast*)=0;
    virtual ast* make_while(ast*,ast*)=0;
    virtual ast* make_float(double)=0;
    virtual ast* selector_default(ast*)=0;
    virtual ast* selector_predicate(ast*)=0;
    virtual ast* selector_condition(ast*)=0;
    virtual void selector_handle_predicate(void)=0;
    virtual selector_node* finish_selector()=0;
    virtual ast* make_seq()=0;
    virtual void finish_seq()=0;
    virtual void add_expr(ast* expr)=0;
    virtual ast* start_list()=0;
    virtual void push_list_element(ast*)=0;
    virtual void push_list_element_with_typehint(ast* n,ast* t)=0;
    virtual void push_symbol_identifier(std::string*)=0;
    virtual ast* finish_list()=0;
    virtual ast* finish_symbol()=0;
    virtual ast* make_empty_list()=0;
    virtual ast* make_single_list(ast*)=0;
    virtual void respond( ast* def, bool abbrev=true, std::ostream& os = std::cout )=0;
    virtual void show_cmd( ast* def, std::ostream& os = std::cout )=0;
    virtual void include_cmd( ast* fname)=0;
    virtual void eval_cmd( ast* fname)=0;
    virtual void cd_cmd( ast* fname)=0;
    virtual ast* make_bool(bool b)=0;
    virtual ast* build_list( ast* list, ast* element)=0;
    virtual void render(ast* node)=0;
    virtual void enable_debug(bool enable=true)=0;
    virtual void enable_trace(bool enable=true)=0;
    virtual void done()=0;
};

#endif
