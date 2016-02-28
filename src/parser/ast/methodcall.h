#ifndef AST_METHODCALL
#define AST_METHODCALL

#include <iostream>
#include <string>
#include <list>
#include <set>
#include <inc/references.h>
#include <parser/ast/ast.h>

class context;

class methodcall_node : public ast
{
public:
    methodcall_node(const std::string&);
    virtual objref raw_evaluate(context*);

    virtual void add_target(ast* pObj);
    virtual void add_param(ast*);
    virtual void required_symbols(std::set<symspec>&) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="",
			    const std::string& label="",
			    std::ostream& out=std::cout) const;
    virtual asttype type() const;
    virtual void direct_subordinates( std::list<ast*>& ) const;

protected:
    const std::string _name;
    ast* _target;
    std::list<ast*> _params;
};

#endif
