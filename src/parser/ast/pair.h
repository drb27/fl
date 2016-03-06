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
    pair_node( const astref&,const astref&);
    virtual ~pair_node() {}
    virtual objref raw_evaluate(context*);
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<symspec>&) const;
    virtual astref first() const { return _first; }
    virtual astref second() const { return _second; }
    virtual asttype type() const;
    virtual void direct_subordinates( std::list<astref>& ) const;

protected:
    astref _first;
    astref _second;
};

#endif
