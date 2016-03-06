#ifndef AST_LIST
#define AST_LIST

#include <iostream>
#include <string>
#include <list>
#include <initializer_list>
#include <inc/references.h>
#include <parser/ast/ast.h>

class context;
class fclass;

class list_node : public ast
{
public:

    list_node(bool isLazy=false) : _is_lazy(isLazy) {}
    list_node( std::initializer_list<astref> l);
    virtual void push_element(astref);
    virtual objref raw_evaluate(context*);    
    virtual fclass* type(context*) const;
    virtual std::list<astref>& raw_elements();
    virtual void required_symbols(std::set<symspec>&) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="", 
			    const std::string& label="",
			    std::ostream& out=std::cout) const;
    virtual asttype type() const;
    virtual void direct_subordinates( std::list<astref>& ) const;

private:
    std::list<astref> _elements;
    bool _is_lazy;
};

#endif
