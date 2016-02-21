#ifndef AST_SELECTOR
#define AST_SELECTOR

#include <string>
#include <list>
#include <inc/references.h>
#include <parser/ast/ast.h>

class context;
class pair_node;

class selector_node : public ast
{
public:
    selector_node( ast* selector);
    virtual objref evaluate(context*);
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<symspec>&) const;

    virtual void add_condition(ast* pair);
    virtual void set_default(ast* defaultExpr);
    virtual bool calls_and_returns( const std::string& fname) const;
    virtual asttype type() const;
    virtual void direct_subordinates( std::list<ast*>& ) const;

protected:
    ast* _selector;
    ast* _default{nullptr};
    std::list<pair_node*> _conditions;

};

#endif
