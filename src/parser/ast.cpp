#include <iostream>
#include <functional>
#include <string>
#include <list>
#include <set>
#include <vector>
#include "ast.h"
#include "ast_nodes.h"
#include <interpreter/object.h>
#include <interpreter/context.h>
#include <interpreter/marshall.h>

using std::list;
using std::set;
using std::function;
using std::vector;

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

const fclass* literal_node::type(context* pContext) const
{
    return &(_object->get_class());
}

void list_node::push_element(ast* pNode)
{
    _elements.push_back(pNode);
}

objref list_node::evaluate(context* pContext)
{
    return objref(nullptr);
}

objref list_node::evaluate(context* pContext) const
{
    return objref(nullptr);
}

std::list<ast*>& list_node::raw_elements()
{
    return _elements;
}

const fclass* list_node::type(context* pContext) const
{

    // Get the types of all the child elements
    set<const fclass*> subtypes;

    for ( auto e : _elements )
    {
	subtypes.insert( e->type(pContext) ); 
    }

    // If there is only one type, that's the type of the list
    // Otherwise, it's a list of objects
    if (subtypes.size()==1)
    {
	typespec subtypespec( (*subtypes.begin())->get_spec().full_name(), {} );
	typespec listspec( "list", {subtypespec} );
	return &(pContext->types().lookup(listspec)); 
    }
    else
    {
	typespec objspec( "object", {} );
	typespec listspec( "list", { pContext->types().lookup(objspec).get_spec() } );
	return &(pContext->types().lookup(listspec)); 
    }
    
}

methodcall_node::methodcall_node(const std::string& name)
    : _name(name)
{

}

objref methodcall_node::evaluate(context* pContext)
{
    // Evaluate the target
    objref target = _target->evaluate(pContext);

    // Look up the method on the class
    function<marshall_fn_t> m = target->get_class().lookup_method(_name);
    
    // Prepare the parameter vector
    auto params = vector<ast*>(_params.size()+1);

    // std::cout << "There are " << _params.size() << " params" << std::endl;

    params[0] = _target;

    int index=1;
    for ( auto p : _params )
    {
	params[index++] = p;
    }
    
    // for ( auto p : params )
    // {
    // 	std::cout << "PARAM: ";
    // 	p->evaluate(pContext)->render(std::cout);
    // 	std::cout << std::endl;
    // }

    // Dispatch the call
    return m(pContext,params);
}

objref methodcall_node::evaluate(context* pContext) const
{
    return objref(nullptr);
}

const fclass* methodcall_node::type(context*) const
{
    throw std::exception();
}

void methodcall_node::add_target(ast* pObj)
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

const fclass* symbol_node::type(context*) const
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

const fclass* assign_node::type(context* c) const
{
    return _lvalue->type(c);
}
