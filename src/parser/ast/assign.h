#ifndef AST_ASSIGN
#define AST_ASSIGN

#include <iostream>
#include <string>
#include <list>
#include <set>
#include <inc/references.h>
#include <parser/ast/ast.h>

class context;
class fclass;

class assign_node : public ast
{
public:
    assign_node(const astref&,const astref&,bool alias=false);
    virtual objref raw_evaluate(context*);
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<symspec>&) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="",
			    const std::string& label="",
			    std::ostream& out=std::cout) const;
    virtual asttype type() const;
    virtual void direct_subordinates( std::list<astref>& ) const;

private:
    astref _lvalue;
    astref _rvalue;
    const bool _alias;
};

#endif
