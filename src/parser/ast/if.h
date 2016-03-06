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
    if_node(const astref& pCondition, 
	    const astref& trueExpression, 
	    const astref& falseExpression);

    virtual objref raw_evaluate(context*);    
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<symspec>&) const;
    virtual bool calls_and_returns( const std::string& fname) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="",
			    const std::string& label="",
			    std::ostream& out=std::cout) const;
    virtual asttype type() const;
    virtual void direct_subordinates( std::list<astref>& ) const;

    virtual astref cond() const { return _condition; }
    virtual astref true_expr() const { return _trueExpr; }
    virtual astref false_expr() const { return _falseExpr; }

protected:
    astref _condition;
    astref _trueExpr;
    astref _falseExpr;
};

#endif
