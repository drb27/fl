#include <string>
#include <set>
#include <list>
#include <vector>
#include <inc/references.h>
#include <parser/ast/selector.h>
#include <parser/ast/pair.h>
#include <interpreter/obj/bool_object.h>
#include <interpreter/eval_exception.h>

using std::string;
using std::list;
using std::set;
using std::vector;

selector_node::selector_node(ast* selector)
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

objref selector_node::evaluate(context* pContext)
{
    // Evaluate the selector
    objref selResult = _selector->evaluate(pContext);

    objref result=objref(nullptr);
    boolref trueObj = boolref( new bool_object(pContext,true));

    // Search the conditions in order
    for ( auto pair : _conditions )
    {
	// Evaluate the guard
	auto firstResult = pair->first()->evaluate(pContext);

	// Compare the guard to the selector value
	vector<objref> params{firstResult};
	boolref is_equal = std::dynamic_pointer_cast<bool_object>(selResult->invoke("eq",pContext,params));
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

void selector_node::required_symbols(set<string>& s ) const
{
    _selector->required_symbols(s);
    if (_default)
	_default->required_symbols(s);

    for ( auto pair : _conditions )
    {
	pair->first()->required_symbols(s);
	pair->second()->required_symbols(s);
    }
}

void selector_node::add_condition(ast* pair)
{
    pair_node* pPairNode = dynamic_cast<pair_node*>(pair);
    _conditions.push_back(pPairNode);
}

void selector_node::set_default(ast* defaultExpr)
{
    _default = defaultExpr;
}
asttype selector_node::type() const
{
    return asttype::selector;
}

void selector_node::direct_subordinates( list<ast*>& subs ) const
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
