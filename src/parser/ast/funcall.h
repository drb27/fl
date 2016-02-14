#ifndef AST_FUNCALL
#define AST_FUNCALL

#include <iostream>
#include <string>
#include <list>
#include <set>
#include <inc/references.h>
#include <parser/ast/ast.h>

class context;
class fclass;

class funcall_node : public ast
{
public:
    funcall_node(const std::string&,ast* args);
    virtual objref evaluate(context*);
    virtual objref evaluate(context*,fnref);
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<std::string>&) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="",
			    const std::string& label="",
			    std::ostream& out=std::cout) const;
    virtual bool calls_and_returns( const std::string& fname) const;
    virtual asttype type() const;
    virtual void direct_subordinates( std::list<ast*>& ) const;

    virtual ast* args() const { return _arg_list; }
    virtual const std::string& name() const { return _name; }

protected:
    const std::string _name;
    ast* _arg_list;
    mutable objref _result;
};

#endif
