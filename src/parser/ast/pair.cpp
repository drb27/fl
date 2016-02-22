#include <string>
#include <set>
#include <list>
#include <inc/references.h>
#include <parser/ast/pair.h>

using std::string;
using std::list;
using std::set;

pair_node::pair_node(ast* f, ast* s )
    : _first(f), _second(s)
{
}

objref pair_node::raw_evaluate(context* pContext)
{
    return _second->evaluate(pContext);
}

fclass* pair_node::type(context* pContext) const
{
    return _second->type(pContext);
}

void pair_node::required_symbols(set<symspec>& s) const
{
    _first->required_symbols(s);
    _second->required_symbols(s);
}

asttype pair_node::type() const
{
    return asttype::pair;
}

void pair_node::direct_subordinates( list<ast*>& subs ) const
{
    subs.push_back(_first);
    subs.push_back(_second);
}
