#ifndef AST_LITERAL
#define AST_LITERAL

#include <string>
#include <list>
#include <inc/references.h>
#include <parser/ast/ast.h>

class context;

class literal_node : public ast
{
public:
    literal_node( objref );
    virtual objref raw_evaluate(context*);
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<symspec>&) const;
    virtual void direct_subordinates( std::list<ast*>& ) const;

protected:
    const objref _object;
    virtual asttype type() const;
};

#endif
