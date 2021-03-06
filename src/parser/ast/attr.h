#ifndef AST_ATTR
#define AST_ATTR

#include <iostream>
#include <string>
#include <list>
#include <functional>
#include <inc/references.h>
#include <parser/ast/ast.h>
#include <parser/ast/lvalue.h>

class context;

class attr_node : public lvalue_node
{
public:
    attr_node(const astref& target, const std::string& selector);
    virtual objref raw_evaluate(context*);
    virtual void required_symbols(std::set<symspec>&) const;
    virtual std::function<void(objref)> setter(context*);
    virtual void render_dot(int& uuid, 
			    const std::string& parent="",
			    const std::string& label="",
			    std::ostream& out=std::cout) const;
    virtual asttype type() const;
    virtual void direct_subordinates( std::list<astref>& ) const;

protected:
    astref _target;
    std::string _selector;
};

#endif
