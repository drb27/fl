#include <string>
#include <set>
#include <list>
#include <inc/references.h>
#include <parser/ast/literal.h>
#include <interpreter/obj/object.h>

using std::string;
using std::set;
using std::list;

literal_node::literal_node(objref pObj)
    : ast(),_object(pObj)
{
}

objref literal_node::evaluate(context* pContext)
{
    return _object;
}

void literal_node::required_symbols(std::set<std::string>&) const
{
}

fclass* literal_node::type(context* pContext) const
{
    return &(_object->get_class());
}

asttype literal_node::type() const
{
    return asttype::literal;
}

void literal_node::direct_subordinates( list<ast*>& subs ) const
{
}
