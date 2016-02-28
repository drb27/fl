#ifndef AST_SEQUENCE
#define AST_SEQUENCE

#include <iostream>
#include <string>
#include <list>
#include <set>
#include <inc/references.h>
#include <parser/ast/ast.h>

class context;
class fclass;

class sequence_node : public ast
{
public:
    sequence_node(bool isolated=false);
    virtual objref raw_evaluate(context*);
    virtual objref evaluate_isolated(context* pContext);
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<symspec>&) const;
    virtual void add_expr(ast*);
    virtual  bool calls_and_returns( const std::string& fname) const;
    virtual asttype type() const;
    virtual void direct_subordinates( std::list<ast*>& ) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="", 
			    const std::string& label="",
			    std::ostream& out=std::cout) const;

protected:
    std::list<ast*> _sequence;
    const bool _isolated;
};

#endif
