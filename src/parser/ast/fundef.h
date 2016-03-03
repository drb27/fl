#ifndef AST_FUNDEF
#define AST_FUNDEF

#include <iostream>
#include <string>
#include <list>
#include <set>
#include <inc/references.h>
#include <parser/ast/ast.h>
#include <parser/ast/list.h>

class context;
class fclass;

class fundef_node : public ast
{
public:
    fundef_node(const astref& arglist, const astref& definition);
    virtual objref raw_evaluate(context*);
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<symspec>&) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="", 
			    const std::string& label="",
			    std::ostream& out=std::cout) const;

    virtual bool is_tail_recursive(const std::string& name) const;
    virtual asttype type() const;
    virtual void direct_subordinates( std::list<astref>& ) const;

    virtual listnoderef args() const 
    { 
	return std::dynamic_pointer_cast<list_node>(_arglist); 
    }
    
    virtual astref def() const { return _definition; }
    virtual void replace_definition(const astref& pNewDef) { _definition = pNewDef; }

protected:

    astref _arglist;
    astref _definition;
};

#endif
