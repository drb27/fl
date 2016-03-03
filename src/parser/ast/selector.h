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
    selector_node( const astref& selector);
    virtual objref raw_evaluate(context*);
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<symspec>&) const;
    virtual void add_condition(const pairnoderef& pair);
    virtual void set_predicate(const astref& pred);
    virtual void set_default(const astref& defaultExpr);
    virtual bool calls_and_returns( const std::string& fname) const;
    virtual asttype type() const;
    virtual void direct_subordinates( std::list<astref>& ) const;

    void use_handle_predicate();
    void set_unwind_stack_frame(colref c) { _unwind_stack_frame=c; }
    colref unwind_stack_frame() { return _unwind_stack_frame; }

protected:

    boolref eval_predicate(context*,objref value, objref guard);
    static const astref& get_handle_predicate();

    astref _selector;
    astref _predicate{nullptr};
    astref _default{nullptr};
    std::list<pairnoderef> _conditions;
    colref _unwind_stack_frame;

private:
    static astref make_handle_predicate();
    static astref _handle_predicate;

};

#endif
