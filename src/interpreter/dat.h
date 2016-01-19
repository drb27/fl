#ifndef DAT_H
#define DAT_H

#include <memory>
#include <deque>
#include <parser/action_target.h>
#include <interpreter/typemgr.h>

class context;
class list_node;

class dat : public action_target
{

 public:
    dat(std::shared_ptr<context>);
    virtual ~dat();

    virtual ast* make_int(int x) const;
    virtual ast* make_null() const;
    virtual ast* make_alias(ast* alias, ast* existing) const;
    virtual ast* make_fundef( ast* arglist,  ast* def) const;
    virtual ast* make_funcall( ast* fn,  ast* args) const;
    virtual ast* make_ifnode( ast* condExpr,  ast* trueExpr, ast* falseExpr) const;
    virtual ast* make_symbol( std::string* name) const;
    virtual void respond( ast* def, std::ostream& os = std::cout ) const;
    virtual ast* make_methodcall( ast* target, std::string* method, list_node* args);
    virtual ast* make_assign_node(ast* lvalue, ast* rvalue);
    virtual ast* start_list();
    virtual void push_list_element(ast*);
    virtual ast* finish_list();
    virtual ast* make_empty_list();
    virtual ast* make_single_list(ast*);
    virtual ast* make_bool(bool b);
    virtual ast* build_list( ast* list, ast* element);
    virtual void render(ast* node);
    virtual void enable_debug(bool enable=true);
 protected:
    std::shared_ptr<context> _context;
    std::deque<list_node*> _list_stack;
 private:

};

#endif
