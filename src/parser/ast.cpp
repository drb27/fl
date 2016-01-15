#include <iostream>
#include <functional>
#include <string>
#include <list>
#include <set>
#include <deque>
#include <vector>
#include "ast.h"
#include "ast_nodes.h"
#include <interpreter/object.h>
#include <interpreter/context.h>
#include <interpreter/marshall.h>

using std::string;
using std::list;
using std::set;
using std::function;
using std::vector;
using std::deque;

ast::ast()
{
}

ast::~ast()
{

}

fclass* ast::type(context* pContext) const
{
    typespec os("object",{});
    return &(pContext->types().lookup(os));
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

fclass* literal_node::type(context* pContext) const
{
    return &(_object->get_class());
}

void list_node::push_element(ast* pNode)
{
    _elements.push_back(pNode);
}

objref list_node::evaluate(context* pContext)
{
    auto l = new list_object(*(type(pContext)));
    for ( auto e : _elements )
    {
	l->internal_value().push_back(e->evaluate(pContext));
    }
    return objref(l);
}

objref list_node::evaluate(context* pContext) const
{
    auto l = new list_object(*(type(pContext)));
    for ( auto e : _elements )
    {
	l->internal_value().push_back(e->evaluate(pContext));
    }
    return objref(l);

}

std::list<ast*>& list_node::raw_elements()
{
    return _elements;
}

fclass* list_node::type(context* pContext) const
{

    // Get the types of all the child elements
    set<fclass*> subtypes;

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

fclass* symbol_node::type(context* pContext) const
{
    // Attempt to resolve the symbol
    auto o = pContext->resolve_symbol(_name);

    if (o)
	return &(o->get_class());

    throw std::exception();
}

assign_node::assign_node(ast* lvalue,ast* rvalue, bool alias)
    : _lvalue(lvalue),_rvalue(rvalue),_alias(alias)
{

}

objref assign_node::evaluate(context* pContext)
{
    auto pSymbolNodeL = dynamic_cast<symbol_node*>(_lvalue);
    if (pSymbolNodeL)
    {
	// Evaluate RHS
	auto result = _rvalue->evaluate(pContext);

	if (_alias)
	{
	    // Set up an alias to point to the same object
	    auto pSymbolNodeR = dynamic_cast<symbol_node*>(_rvalue);
	    pContext->alias(pSymbolNodeL,pSymbolNodeR);
	}
	else
	    // Assign the LHS to the symbol
	    pContext->assign(pSymbolNodeL->name(),result);

	// Return RHS
	return result;
       
    }
    else
	throw std::exception();
}

objref assign_node::evaluate(context* pContext) const
{
    auto pSymbolNodeL = dynamic_cast<symbol_node*>(_lvalue);
    if (pSymbolNodeL)
    {
	// Evaluate RHS
	auto result = _rvalue->evaluate(pContext);

	if (_alias)
	{
	    // Set up an alias to point to the same object
	    auto pSymbolNodeR = dynamic_cast<symbol_node*>(_rvalue);
	    pContext->alias(pSymbolNodeL,pSymbolNodeR);
	}
	else
	    // Assign the LHS to the symbol
	    pContext->assign(pSymbolNodeL->name(),result);

	// Return RHS
	return result;
       
    }
    else
	throw std::exception();

}

fclass* assign_node::type(context* c) const
{
    return _lvalue->type(c);
}

fundef_node::fundef_node(ast* arglist, ast* definition)
    : _arglist(arglist), _definition(definition)
{

}

objref fundef_node::evaluate(context* pContext) const
{
    ast* localDef = _definition;
    typespec ts("function",{});
    fclass& cls = pContext->types().lookup(ts);
    
    deque<string> argnames;

    list_node* pArgList = dynamic_cast<list_node*>(_arglist);
    for ( auto sn : pArgList->raw_elements() )
    {
	symbol_node* pSymNode = dynamic_cast<symbol_node*>(sn);
	argnames.push_back(pSymNode->name());
    }

    // Construct a marshall_fn_t compatible lambda expression
    function<marshall_fn_t> fn = [localDef](context* pContext, vector<ast*>& arglist)
	{
	    return localDef->evaluate(pContext);
	};

    return objref( new fn_object(cls,fn,argnames) );
}

objref fundef_node::evaluate(context* pContext)
{
    ast* localDef = _definition;
    typespec ts("function",{});
    fclass& cls = pContext->types().lookup(ts);
    
    deque<string> argnames;

    list_node* pArgList = dynamic_cast<list_node*>(_arglist);
    for ( auto sn : pArgList->raw_elements() )
    {
	symbol_node* pSymNode = dynamic_cast<symbol_node*>(sn);
	argnames.push_back(pSymNode->name());
    }

    // Construct a marshall_fn_t compatible lambda expression
    function<marshall_fn_t> fn = [localDef](context* pContext, vector<ast*>& arglist)
	{
	    return localDef->evaluate(pContext);
	};

    return objref( new fn_object(cls,fn,argnames) );
    
}

fclass* fundef_node::type(context* pContext) const
{
    typespec ts("function",{});
    return &(pContext->types().lookup(ts));
}

funcall_node::funcall_node(const string& name, ast* args)
    : _name(name), _arg_list(args)
{
}

objref funcall_node::evaluate(context* pContext)
{
    // Look up the function object in the context
    fnref fn = std::dynamic_pointer_cast<fn_object>(pContext->resolve_symbol(_name));

    // Evaluate the argument list
    listref args = std::dynamic_pointer_cast<list_object>(_arg_list->evaluate(pContext));

    // Get a list of argument names expected by the function
    auto argnames(fn->arglist());

    // Construct the argpair vector (string,objref)
    vector<fn_object::argpair_t> argpairs;

    for ( auto argval : args->internal_value() )
    {
	string argname = argnames.front();
	argnames.pop_front();
	argpairs.push_back( fn_object::argpair_t(argname,argval)); 
    }

    // Call the function and return the result!
    return (*fn)(pContext,argpairs);

}

objref funcall_node::evaluate(context* pContext) const
{
    // Look up the function object in the context
    fnref fn = std::dynamic_pointer_cast<fn_object>(pContext->resolve_symbol(_name));

    // Evaluate the argument list
    listref args = std::dynamic_pointer_cast<list_object>(_arg_list->evaluate(pContext));

    // Get a list of argument names expected by the function
    // THE REASON THIS CRASHES IS THAT FUNCTION SYMBOLS ARE NOT PASSED INTO
    // OTHER FUNCTION'S CALL CONTEXT
    auto argnames(fn->arglist());

    // Construct the argpair vector (string,objref)
    vector<fn_object::argpair_t> argpairs;

    for ( auto argval : args->internal_value() )
    {
	string argname = argnames.front();
	argnames.pop_front();
	argpairs.push_back( fn_object::argpair_t(argname,argval) ); 
    }

    // Call the function and return the result!
    return (*fn)(pContext,argpairs);
}

fclass* funcall_node::type(context* pContext) const
{
    // TODO
    typespec obj_spec("object",{});
    return &(pContext->types().lookup(obj_spec));
    //throw std::exception();
}

if_node::if_node(ast* pCondition, ast* trueExpression, ast* falseExpression)
    : _condition(pCondition), _trueExpr(trueExpression), _falseExpr(falseExpression)
{
}

objref if_node::evaluate(context* pContext) const
{
    boolref cond = std::dynamic_pointer_cast<bool_object>(_condition->evaluate(pContext));
    
    if (cond->internal_value())
	return objref(_trueExpr->evaluate(pContext));
    else
	return objref(_falseExpr->evaluate(pContext));
}

objref if_node::evaluate(context* pContext)
{
    boolref cond = std::dynamic_pointer_cast<bool_object>(_condition->evaluate(pContext));
    
    if (cond->internal_value())
	return objref(_trueExpr->evaluate(pContext));
    else
	return objref(_falseExpr->evaluate(pContext));
}

fclass* if_node::type(context* pContext) const
{
    typespec ts("object",{});
    return &(pContext->types().lookup(ts));
}
