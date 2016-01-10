
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

methodcall_node::methodcall_node(const std::string& name)
    : _name(name)
{

}

objref methodcall_node::evaluate(context* pContext)
{
    return objref(nullptr); 
}

objref methodcall_node::evaluate(context* pContext) const
{
    return objref(nullptr);
}

const fclass& methodcall_node::type(context*) const
{
    throw std::exception();
}

void methodcall_node::add_target(objref pObj)
{
    _target = pObj;
}

void methodcall_node::add_param(ast* pNode)
{
    _params.push_back(pNode);
}

void methodcall_node::finalize_params()
{

}

symbol_node::symbol_node(const std::string& name)
    : _name(name)
{

}

const std::string& symbol_node::name() const
{
    return _name;
}

objref symbol_node::evaluate(context* pContext)
{
    return pContext->resolve_symbol(_name); 
}

objref symbol_node::evaluate(context* pContext) const
{
    return pContext->resolve_symbol(_name);
}

const fclass& symbol_node::type(context*) const
{
    throw std::exception();
}

assign_node::assign_node(ast* lvalue,ast* rvalue)
    : _lvalue(lvalue),_rvalue(rvalue)
{

}

objref assign_node::evaluate(context* pContext)
{
    auto pSymbolNode = dynamic_cast<symbol_node*>(_lvalue);
    if (pSymbolNode)
    {
	// Evaluate RHS
	auto result = _rvalue->evaluate(pContext);
	
	// Assign the LHS to the symbol
	pContext->assign(pSymbolNode->name(),result);

	// Return RHS
	return result;
       
    }
    else
	throw std::exception();
}

objref assign_node::evaluate(context* pContext) const
{
    auto pSymbolNode = dynamic_cast<symbol_node*>(_lvalue);
    if (pSymbolNode)
    {
	// Evaluate RHS
	auto result = _rvalue->evaluate(pContext);
	
	// Assign the LHS to the symbol
	pContext->assign(pSymbolNode->name(),result);

	// Return RHS
	return result;
       
    }
    else
	throw std::exception();

}

const fclass& assign_node::type(context* c) const
{
    return _lvalue->type(c);
}
