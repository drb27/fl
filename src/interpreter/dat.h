#ifndef DAT_H
#define DAT_H

#include <parser/action_target.h>
#include <interpreter/typemgr.h>

class context;

class dat : public action_target
{

 public:
    dat(typemgr&,context*);
    virtual ~dat();

    virtual ast* make_int(int x) const;
    virtual ast* make_fundef( std::string* name, ast* def) const;
    virtual ast* make_symbol( std::string* name) const;
    virtual void respond( ast* def, std::ostream& os = std::cout ) const;
    virtual ast* make_methodcall( std::string* target, std::string* method);
    virtual ast* make_assign_node(ast* lvalue, ast* rvalue);
 protected:
    typemgr& _tm;
    context* _context;
 private:

};

#endif
