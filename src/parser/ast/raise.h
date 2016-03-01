#ifndef AST_RAISE_H
#define AST_RAISE_H

#include <iostream>
#include <string>
#include <list>
#include <inc/references.h>
#include <parser/ast/ast.h>

class context;
class fclass;

class raise_node : public ast
{
public:
    raise_node(ast* classExpr);
    virtual objref raw_evaluate(context*);    
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<symspec>&) const;
    virtual bool calls_and_returns( const std::string& fname) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="",
			    const std::string& label="",
			    std::ostream& out=std::cout) const;
    virtual asttype type() const;
    virtual void direct_subordinates( std::list<ast*>& ) const;

    virtual ast* sigobj_expr() const { return _sigObjExpr; }

protected:

    ast* _sigObjExpr;
};

#endif
