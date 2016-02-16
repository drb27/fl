#ifndef DAT_H
#define DAT_H

#include <memory>
#include <deque>
#include <functional>
#include <parser/action_target.h>
#include <interpreter/typemgr.h>

class context;
class list_node;
class sequence_node;
class selector_node;

class dat : public action_target
{

 public:
    dat(context*,
	std::function<void(const std::string&)>,
	std::function<void(const std::string&)>
	);
    virtual ~dat();

    virtual ast* make_int(int x) const;
    virtual ast* make_string(std::string* x) const;
    virtual ast* make_null() const;
    virtual ast* make_float(double);
    virtual ast* make_alias(ast* alias, ast* existing) const;
    virtual ast* make_fundef( ast* arglist,  ast* def) const;
    virtual ast* make_funcall( ast* fn,  ast* args) const;
    virtual ast* make_ifnode( ast* condExpr,  ast* trueExpr, ast* falseExpr) const;
    virtual ast* make_symbol( std::string* name) const;
    virtual void respond( ast* def, bool abbrv = true, std::ostream& os = std::cout ) const;
    virtual void show_cmd( ast* def, std::ostream& os = std::cout );
    virtual void include_cmd( ast* fname);
    virtual void eval_cmd( ast* stmtString);
    virtual ast* make_methodcall( ast* target, ast* method, list_node* args);
    virtual ast* make_assign_node(ast* lvalue, ast* rvalue,bool);
    virtual ast* make_attr( ast* target, std::string* selector);
    virtual ast* make_seq();
    virtual ast* make_equality(ast*,ast*);
    virtual ast* make_index(ast* target, ast* index);
    virtual ast* make_enum_class(std::string*,ast*);
    virtual ast* make_new_class(std::string*,ast*);
    virtual ast* make_while(ast*,ast*);
    virtual void add_expr(ast* expr);
    virtual void finish_seq();
    virtual ast* start_list();
    virtual void push_list_element(ast*);
    virtual void push_list_element_with_typehint(ast* n,ast* t);
    virtual ast* finish_list();
    virtual ast* make_empty_list();
    virtual ast* make_single_list(ast*);
    virtual ast* make_bool(bool b);
    virtual ast* build_list( ast* list, ast* element);
    virtual void render(ast* node);
    virtual void enable_debug(bool enable=true);
    virtual void enable_trace(bool enable=true);
    virtual void done();
    virtual ast* make_pair(ast*,ast*);
    virtual ast* make_selector(ast*);
    virtual ast* selector_default(ast*);
    virtual ast* selector_condition(ast*);
    virtual ast* finish_selector();

 protected:
    context* _context;
    std::deque<list_node*> _list_stack;
    std::deque<sequence_node*> _seq_stack;
    std::deque<selector_node*> _sel_stack;
    std::function<void(const std::string&)> _include_fn;
    std::function<void(const std::string&)> _eval_fn;
 private:

};

#endif
