
#include <string>
#include <list>
#include <set>
#include "ast.h"
#include <interpreter/object.h>
#include <interpreter/context.h>

using std::list;
using std::set;

ast::ast()
{
}

ast::~ast()
{

}

literal_node::literal_node(objref pObj)
    : ast(),_object(pObj)
{
}

objref literal_node::evaluate(context* pContext)
{
    return _object;
}

objref literal_node::evaluate(context* pContext) const
{
    return _object;
}

const fclass& literal_node::type(context* pContext) const
{
    return _object->get_class();
}

void list_node::push_element(ast* pNode)
{
    _elements.push_back(pNode);
}

objref list_node::evaluate(context* pContext)
{
    return literal_node::evaluate(pContext);
}

const fclass& list_node::type(context* pContext) const
{

    // Get the types of all the child elements
    set<const fclass*> subtypes;

    for ( auto e : _elements )
    {
	subtypes.insert( &e->type(pContext) ); 
    }

    // If there is only one type, that's the type of the list
    // Otherwise, it's a list of objects
    if (subtypes.size()==1)
    {
	typespec subtypespec( (*subtypes.begin())->get_spec().full_name(), {} );
	typespec listspec( "list", {subtypespec} );
	return pContext->types().lookup(listspec); 
    }
    else
    {
	typespec objspec( "object", {} );
	typespec listspec( "list", { pContext->types().lookup(objspec).get_spec() } );
	return pContext->types().lookup(listspec); 
    }
    
}
