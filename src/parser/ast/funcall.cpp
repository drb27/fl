#include <cassert>
#include <string>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <sstream>
#include <inc/references.h>
#include <parser/ast/funcall.h>
#include <interpreter/obj/fn_object.h>
#include <interpreter/obj/list_object.h>
#include <interpreter/obj/class_object.h>
#include <logger/logger.h>
#include <interpreter/eval_exception.h>
#include <interpreter/context.h>

using std::string;
using std::list;
using std::set;
using std::vector;
using std::map;
using std::stringstream;

funcall_node::funcall_node(const symspec& ss, ast* args, fnref pFn)
    : _symbol(ss), _arg_list(args),_captured_fn(pFn)
{
    wlog_entry();
    wlog_exit();
}

bool funcall_node::calls_and_returns( const string& fname) const
{
    return _symbol.name()==fname;
}

void funcall_node::render_dot(int& uuid, 
			     const string& parent,
			     const string& label,
			     std::ostream& out) const
{
    int myref=uuid++;
    string labelString;

    string myid = "funcall_" + _symbol.name() + "_" + std::to_string(myref);
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    out << myid << "[shape=box" << labelString << "];" << std::endl;

    _arg_list->render_dot(uuid,myid," lvalue",out);
}

void funcall_node::required_symbols(set<symspec>& s) const
{
    // Add all the arguments
    _arg_list->required_symbols(s);

    // ... and the name of the added function
    s.insert(_symbol.name());
}

objref funcall_node::raw_evaluate(context* pContext)
{
    auto class_cls = builtins::flclass::get_class();

    // The function can either be captured (embedded in this object), or
    // looked up in the context.
    fnref fn;

    if (pContext->is_defined(_symbol) )
    {
	fn = object::cast_or_abort<fn_object>(pContext->resolve_symbol(_symbol));
	_captured_fn = fn;
    }
    else
	fn = _captured_fn;

    assert(fn);

    // Evaluate the argument list
    listref args = object::cast_or_abort<list_object>(_arg_list->evaluate(pContext));

    // Get a list of argument names expected by the function
    auto argnames(fn->arglist());

    if (argnames.size()< args->size() )
	throw eval_exception(cerror::too_many_arguments, "Too many arguments supplied to function call");

    // Construct the argpair vector (string,objref)
    vector<fn_object::argpair_t> argpairs;

    for ( int index=0; index < args->size() ; index++ )
    {
	wlog(level::debug,argnames.front().first);
	string argname = argnames.front().first;

	// Does this argument have a type hint?
	if ( argnames.front().second )
	{
	    // Yes. Evaluate it
	    objref pTypeIntObj = argnames.front().second->evaluate(pContext);

	    // Does it evaluate to a class object?
	    if ( &(pTypeIntObj->get_class()) == class_cls )
	    {
		// Yes - cast to a class object
		classref pTypeHintCls = std::dynamic_pointer_cast<class_object>(pTypeIntObj);

		// Convert to that class
		argpairs.push_back( fn_object::argpair_t( argname,
							  object::convert_to( args->get_element(index),
									      pTypeHintCls->internal_value() )));
		
	    }
	    else
	    {
		throw eval_exception(cerror::not_a_class,
				     "Type hint does not evaluate to a class object" );
	    }
	}
	else
	{
	    argpairs.push_back( fn_object::argpair_t(argname,args->get_element(index)));
	}

	argnames.pop_front();

    }

    map<string,string> trace_params;
    stringstream s;
    for ( auto p : argpairs)
    {
	p.second->render(s,true);
	trace_params[p.first] = s.str();
	s.str("");
    }

    // Call the function and return the result!
    wlog_trace("CALL: " + _symbol.name(),trace_params);
    auto retVal =  (*fn)(pContext,argpairs);
    wlog_exit();
    return retVal;

}

fclass* funcall_node::type(context* pContext) const
{
    return builtins::object::get_class();
}

asttype funcall_node::type() const
{
    return asttype::funcall;
}

void funcall_node::direct_subordinates( list<ast*>& subs ) const
{
    subs.push_back(_arg_list);
    if (_captured_fn && _captured_fn->raw().def() )
	_captured_fn->raw().def()->direct_subordinates(subs);
}
