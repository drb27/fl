#ifndef AST_SYMBOL
#define AST_SYMBOL

#include <string>
#include <list>
#include <set>
#include <parser/ast/ast.h>
#include <parser/ast/lvalue.h>

class context;
class fclass;

class symbol_node : public lvalue_node
{
public:
    symbol_node( const std::string& );
    virtual objref evaluate(context*);
    virtual fclass* type(context*) const;
    virtual const std::string& name() const;
    virtual void required_symbols(std::set<std::string>&) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="", 
			    const std::string& label="",
			    std::ostream& out=std::cout) const;
    virtual std::function<void(objref)> setter(context*);
    virtual asttype type() const;
    virtual void direct_subordinates( std::list<ast*>& ) const;

protected:
    const std::string _name;

};


#endif
