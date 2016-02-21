#ifndef AST_LIST
#define AST_LIST

#include <iostream>
#include <string>
#include <list>
#include <inc/references.h>
#include <parser/ast/ast.h>

class context;
class fclass;

class list_node : public ast
{
public:
    virtual void push_element(ast*);
    virtual objref evaluate(context*);    
    virtual fclass* type(context*) const;
    virtual std::list<ast*>& raw_elements();
    virtual void required_symbols(std::set<symspec>&) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="", 
			    const std::string& label="",
			    std::ostream& out=std::cout) const;
    virtual asttype type() const;
    virtual void direct_subordinates( std::list<ast*>& ) const;

private:
    std::list<ast*> _elements;
};

#endif