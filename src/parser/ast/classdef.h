#ifndef AST_CLASSDEF
#define AST_CLASSDEF

#include <iostream>
#include <string>
#include <list>
#include <set>
#include <inc/references.h>
#include <parser/ast/ast.h>

class context;

class classdef_node : public ast
{
public:
    classdef_node(const std::string& name, ast* pDeriveList);
    virtual objref evaluate(context*);
    virtual void required_symbols(std::set<symspec>&) const;

    virtual asttype type() const;
    virtual void direct_subordinates( std::list<ast*>& ) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="", 
			    const std::string& label="",
			    std::ostream& out=std::cout) const;

private:

    ast* _derive_list;
    const std::string _name;
};

#endif
