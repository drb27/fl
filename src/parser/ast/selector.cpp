#include <string>
#include <set>
#include <list>
#include <vector>
#include <inc/references.h>
#include <parser/ast/selector.h>
#include <parser/ast/pair.h>
#include <parser/ast/list.h>
#include <parser/ast/literal.h>
#include <parser/ast/funcall.h>
#include <parser/ast/fundef.h>
#include <parser/ast/methodcall.h>
#include <parser/ast/symbol.h>
#include <interpreter/obj/bool_object.h>
#include <interpreter/obj/fn_object.h>
#include <interpreter/eval_exception.h>

using std::string;
using std::list;
using std::set;
using std::vector;

astref selector_node::_handle_predicate{nullptr};

selector_node::selector_node(const astref& selector)
    : _selector(selector)
{
}

bool selector_node::calls_and_returns( const std::string& fname) const
{
    // True if any of the actions (including the default action) return true
    for ( auto pair : _conditions )
    {
	if ( pair->second()->calls_and_returns(fname) )
	    return true;
    }

    if ( _default && ( _default->calls_and_returns(fname) ) )
	return true;

    return false;
}

boolref selector_node::eval_predicate(context* pContext,
				      objref value,
				      objref guard)
{
    if ( !_predicate)
    {
	vector<objref> params{value};
	return object::cast_or_abort<bool_object>
	    (guard->invoke(".selmatch",pContext,params));
    }
    else
    {
	// Evaluate the predicate expression to yield a function
	fnref pPredicate = object::cast_or_abort<fn_object>
	    ( _predicate->evaluate(pContext) );

	// Call the function with (guard,value) as parameters
	listnoderef pArgList(new list_node());
	literal_node* pGuardArg = new literal_node(guard);
	literal_node* pValueArg = new literal_node(value);

	pArgList->push_element(astref(pGuardArg));
	pArgList->push_element(astref(pValueArg));

	funcall_node* pFnCall = new funcall_node(symspec("(anonymous)"), 
						 pArgList,
						 pPredicate);

	auto result = object::cast_or_abort<bool_object>
	    ( pFnCall->evaluate(pContext) );

	delete pFnCall;
	return result;
    }
}

objref selector_node::raw_evaluate(context* pContext)
{
    // Evaluate the selector
    objref selResult = _selector->evaluate(pContext);

    objref result=objref(nullptr);
    boolref trueObj = boolref( new bool_object(pContext,true));

    // Search the conditions in order
    for ( auto pair : _conditions )
    {
	// Evaluate the guard
	auto guardResult = pair->first()->evaluate(pContext);

	// Compare the guard to the selector value
	boolref is_equal = eval_predicate(pContext,selResult,guardResult);

	if ( is_equal->internal_value() )
	{
	    // We have a match!
	    result = pair->second()->evaluate(pContext);
	    break;
	}   
    }
    
    // Match found?
    if (result)
    {
	// Yes, return it now
	return result;
    }
    else
    {
	// No match - do we have a default?
	if (_default)
	{
	    // Evaluate and return the default
	    return _default->evaluate(pContext);
	}
	else
	{
	    // Failed to match anything
	    throw eval_exception(cerror::selector_nomatch, "Selector failed to match anything");
	}
    }

    // TODO
    return objref(nullptr);
}

fclass* selector_node::type(context* pContext) const
{
    return builtins::object::get_class();
}

void selector_node::required_symbols(set<symspec>& s ) const
{
    _selector->required_symbols(s);
    if (_default)
	_default->required_symbols(s);

    for ( auto pair : _conditions )
    {
	pair->first()->required_symbols(s);
	pair->second()->required_symbols(s);
    }

    if (_predicate)
	_predicate->required_symbols(s);
}

void selector_node::add_condition(const pairnoderef& pair)
{
    _conditions.push_back(pair);
}

void selector_node::set_default(const astref& defaultExpr)
{
    _default = defaultExpr;
}
asttype selector_node::type() const
{
    return asttype::selector;
}

void selector_node::direct_subordinates( list<astref>& subs ) const
{
    subs.push_back(_selector);

    if (_default)
	subs.push_back(_default);

    for (auto p : _conditions )
    {
	subs.push_back(p->first());
	subs.push_back(p->second());
    }
}

void selector_node::use_handle_predicate() 
{
    _predicate = get_handle_predicate();
}

void selector_node::set_predicate(const astref& pred )
{
    _predicate = pred;
}

astref selector_node::make_handle_predicate()
{
    // Arguments
    list_node* arglist = 
	new list_node({astref(new symbol_node("guard")), 
		    astref(new symbol_node("value"))});

    // Arg to method call
    list_node* pMethodCallArgs = new list_node();
    pMethodCallArgs->push_element( astref(new symbol_node("value" )) );

    // Function body
    methodcall_node* definition = new methodcall_node("member");
    definition->add_target( astref(new symbol_node("guard") ));
    definition->add_param_list(astref(pMethodCallArgs));

    return astref(new fundef_node(astref(arglist),astref(definition)));
}

const astref& selector_node::get_handle_predicate()
{
    if (!_handle_predicate)
	_handle_predicate = make_handle_predicate();

    return _handle_predicate;
}
