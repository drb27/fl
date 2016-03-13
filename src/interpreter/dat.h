#ifndef DAT_H
#define DAT_H

#include <memory>
#include <list>
#include <deque>
#include <functional>
#include <parser/action_target.h>
#include <interpreter/typemgr.h>

class package;
class list_node;
class sequence_node;
class selector_node;

class dat : public action_target
{

 public:
    dat(package*,
	std::function<void(const std::string&)>,
	std::function<void(const std::string&)>
	);
    virtual ~dat();

    virtual ast* make_range(const astref& a, const astref& b);
    virtual ast* make_raise(const astref& clsExpr);
    virtual ast* make_int(int x) const;
    virtual ast* make_lazy(const astref& e);
    virtual ast* make_string(std::string* x) const;
    virtual ast* make_null() const;
    virtual ast* make_float(double);
    virtual ast* make_alias(const astref& alias, const astref& existing) const;
    virtual ast* make_fundef( const astref& arglist,  const astref& def) const;
    virtual ast* make_funcall( const astref& fn,  const astref& args) const;
    virtual ast* make_ifnode( const astref& condExpr,  const astref& trueExpr, const astref& falseExpr) const;
    virtual ast* make_symbol( std::string* name, const std::list<std::string>& scopespec = {}) const;
    virtual void switch_package( const astref& symbol );
    virtual void respond( const astref& def, bool abbrv = true, std::ostream& os = std::cout );
    virtual void show_cmd( const astref& def, std::ostream& os = std::cout );
    virtual void include_cmd( const astref& fname);
    virtual void eval_cmd( const astref& stmtString);
    virtual void cd_cmd( const astref& fname);
    virtual void nodecount_cmd();
    virtual ast* make_methodcall( const astref& target, const astref& method, const astref& args);
    virtual ast* make_assign_node(const astref& lvalue, const astref& rvalue,bool);
    virtual ast* make_attr( const astref& target, std::string* selector);
    virtual ast* make_seq();
    virtual ast* make_equality(const astref&,const astref&);
    virtual ast* make_index(const astref& target, const astref& index);
    virtual ast* make_enum_class(std::string*,const astref&);
    virtual ast* make_new_class(std::string*,const astref&);
    virtual ast* make_while(const astref&,const astref&);
    virtual void add_expr(const astref& expr);
    virtual void finish_seq();
    virtual ast* start_list();
    virtual void push_list_element(const astref&);
    virtual void push_list_element_with_typehint(const astref& n,const astref& t);
    virtual void push_symbol_identifier(std::string*);
    virtual ast* finish_list();
    virtual ast* finish_symbol();
    virtual ast* make_empty_list();
    virtual ast* make_single_list(const astref&);
    virtual ast* make_bool(bool b);
    virtual ast* build_list( const astref& list, const astref& element);
    virtual void render(const astref& node);
    virtual void enable_debug(bool enable=true);
    virtual void enable_trace(bool enable=true);
    virtual void done();
    virtual ast* make_pair(const astref&,const astref&);
    virtual ast* make_selector(const astref&);
    virtual ast* start_observed_expression();
    virtual void selector_default(const astref&);
    virtual void selector_predicate(const astref&);
    virtual void selector_condition(const astref&);
    virtual void selector_handle_predicate(void);
    virtual selector_node* finish_selector();

    virtual void push_package();
    virtual void pop_package();

 protected:
    package* _current_pkg;
    package* _root_pkg;
    std::deque<list_node*> _list_stack;
    std::deque<sequence_node*> _seq_stack;
    std::deque<selector_node*> _sel_stack;
    std::list<std::string> _symbol_stack;
    std::function<void(const std::string&)> _include_fn;
    std::function<void(const std::string&)> _eval_fn;
    std::deque<package*> _pkg_stack;
 private:

};

#endif
