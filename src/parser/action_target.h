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

    virtual ast* make_range(const astref& a, const astref& b)=0;
    virtual ast* make_lazy(const astref& e)=0;
    virtual ast* make_raise(const astref& clsExpr)=0;
    virtual ast* make_int(int x) const=0;
    virtual ast* make_string(std::string* x) const=0;
    virtual ast* make_null() const=0;
    virtual ast* make_fundef(const astref& arglist, const astref& def) const=0;
    virtual ast* make_funcall(const astref& fn, const astref& args) const=0;
    virtual ast* make_ifnode(const astref& condExpr,  const astref& trueExpr, const astref& falseExpr) const=0;
    virtual ast* make_symbol( std::string* name, const std::list<std::string>& scopespec={}) const=0;
    virtual void switch_package(const astref& symbol )=0;
    virtual ast* make_alias(const astref& alias, const astref& existing) const=0;
    virtual ast* make_assign_node(const astref& lvalue, const astref& rvalue,bool alias=false)=0;
    virtual ast* make_methodcall(const astref& target, const astref& method, const astref& args)=0;
    virtual ast* make_attr(const astref& target, std::string* selector)=0;
    virtual ast* make_pair(const astref&,const astref&)=0;
    virtual ast* make_selector(const astref&)=0;
    virtual ast* start_observed_expression()=0;
    virtual ast* make_equality(const astref&,const astref&)=0;
    virtual ast* make_index(const astref&,const astref&)=0;
    virtual ast* make_enum_class(std::string*,const astref&)=0;
    virtual ast* make_new_class(std::string*,const astref&)=0;
    virtual ast* make_while(const astref&,const astref&)=0;
    virtual ast* make_float(double)=0;
    virtual void selector_default(const astref&)=0;
    virtual void selector_predicate(const astref&)=0;
    virtual void selector_condition(const astref&)=0;
    virtual void selector_handle_predicate(void)=0;
    virtual selector_node* finish_selector()=0;
    virtual ast* make_seq()=0;
    virtual void finish_seq()=0;
    virtual void add_expr(const astref& expr)=0;
    virtual ast* start_list()=0;
    virtual void push_list_element(const astref&)=0;
    virtual void push_list_element_with_typehint(const astref& n,const astref& t)=0;
    virtual void push_symbol_identifier(std::string*)=0;
    virtual ast* finish_list()=0;
    virtual ast* finish_symbol()=0;
    virtual ast* make_empty_list()=0;
    virtual ast* make_single_list(const astref&)=0;
    virtual void respond(const astref& def, bool abbrev=true, std::ostream& os = std::cout )=0;
    virtual void show_cmd(const astref& def, std::ostream& os = std::cout )=0;
    virtual void include_cmd(const astref& fname)=0;
    virtual void eval_cmd(const astref& fname)=0;
    virtual void cd_cmd(const astref& fname)=0;
    virtual void nodecount_cmd()=0;
    virtual ast* make_bool(bool b)=0;
    virtual ast* build_list(const astref& list, const astref& element)=0;
    virtual void render(const astref& node)=0;
    virtual void enable_debug(bool enable=true)=0;
    virtual void enable_trace(bool enable=true)=0;
    virtual void done()=0;
};

#endif
