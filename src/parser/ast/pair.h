#ifndef AST_PAIR
#define AST_PAIR

#include <string>
#include <list>
#include <set>
#include <inc/references.h>
#include <parser/ast/ast.h>

class context;
class fclass;

class pair_node : public ast
{
public:
    pair_node( ast*,ast*);
    virtual objref evaluate(context*);
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<symspec>&) const;
    virtual ast* first() const { return _first; }
    virtual ast* second() const { return _second; }
    virtual asttype type() const;
    virtual void direct_subordinates( std::list<ast*>& ) const;

protected:
    ast* _first;
    ast* _second;
};

#endif
