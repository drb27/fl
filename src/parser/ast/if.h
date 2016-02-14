#ifndef AST_IF
#define AST_IF

#include <iostream>
#include <string>
#include <list>
#include <inc/references.h>
#include <parser/ast/ast.h>

class context;
class fclass;

class if_node : public ast
{
public:
    if_node(ast* pCondition, ast* trueExpression, ast* falseExpression);
    virtual objref evaluate(context*);    
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<std::string>&) const;
    virtual bool calls_and_returns( const std::string& fname) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="",
			    const std::string& label="",
			    std::ostream& out=std::cout) const;
    virtual asttype type() const;
    virtual void direct_subordinates( std::list<ast*>& ) const;

    virtual ast* cond() const { return _condition; }
    virtual ast* true_expr() const { return _trueExpr; }
    virtual ast* false_expr() const { return _falseExpr; }

protected:
    ast* _condition;
    ast* _trueExpr;
    ast* _falseExpr;
};

#endif
