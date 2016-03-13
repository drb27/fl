#ifndef AST_RANGE
#define AST_RANGE

#include <parser/ast/pair.h>

class range_node : public pair_node
{
public:
    range_node( const astref&,const astref&);
    virtual objref raw_evaluate(context*);
    virtual asttype type() const;

};

#endif
