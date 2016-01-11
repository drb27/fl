#ifndef DAT_H
#define DAT_H

#include <deque>
#include <parser/action_target.h>
#include <interpreter/typemgr.h>

using std::deque;

class context;
class list_node;

class dat : public action_target
{

 public:
    dat(typemgr&,context*);
    virtual ~dat();

    virtual ast* make_int(int x) const;
    virtual ast* make_null() const;
    virtual ast* make_fundef( std::string* name, ast* def) const;
    virtual ast* make_symbol( std::string* name) const;
    virtual void respond( ast* def, std::ostream& os = std::cout ) const;
    virtual ast* make_methodcall( ast* target, std::string* method, list_node* args);
    virtual ast* make_assign_node(ast* lvalue, ast* rvalue);
    virtual ast* start_list();
    virtual void push_list_element(ast*);
    virtual ast* finish_list();
    virtual ast* make_bool(bool b);
 protected:
    typemgr& _tm;
    context* _context;
    std::deque<list_node*> _list_stack;
 private:

};

#endif
