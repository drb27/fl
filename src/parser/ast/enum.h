#ifndef AST_ENUM
#define AST_ENUM

#include <iostream>
#include <string>
#include <list>
#include <set>
#include <inc/references.h>
#include <parser/ast/ast.h>

class context;

class enum_node : public ast
{
public:
    enum_node(const std::string& name, const astref& pDefList);
    virtual objref raw_evaluate(context*);
    virtual void required_symbols(std::set<symspec>&) const;

    virtual asttype type() const;
    virtual void direct_subordinates( std::list<astref>& ) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="", 
			    const std::string& label="",
			    std::ostream& out=std::cout) const;

private:

    astref _def_list;
    const std::string _name;
};

#endif
