#include <iostream>
#include <string>
#include <parser/ast/ast.h>
#include <interpreter/context.h>
#include <parser/ast/selector.h>
#include <interpreter/builtins.h>
#include <interpreter/obj/signal_object.h>

class context;

using std::ostream;
using std::string;
using std::map;
using std::list;

unsigned long ast::_count{0};

#ifdef AST_MONITOR_LEAKS
std::set<ast*> ast::_leakSet;
#endif

ast::ast()
{
    _count++;

    #ifdef AST_MONITOR_LEAKS
    monitor_for_leak(this);
    #endif
}

ast::~ast()
{
    _count--;
    #ifdef AST_MONITOR_LEAKS
    clear_from_leak_set(this);
    #endif

}

bool ast::calls_and_returns( const string& name) const
{
    return false;
}

void ast::render_dot(int& uuid, 
		     const string& parent, 
		     const string& label,
		     ostream& os) const
{
    int myref = uuid++;

    string labelString;
    if (!parent.empty())
    {
	os << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    os << "astnode_" << myref << "[shape=box" << labelString << "];" << std::endl;
}

fclass* ast::type(context* pContext) const
{
    return builtins::object::get_class();
}

objref ast::evaluate(context* pContext )
{
    _signal = nullptr;

    if (_observer)
	_observer->set_unwind_stack_frame(pContext->current_collection());

    objref result;
    try
    {
	result = raw_evaluate(pContext);
    }
    catch ( eval_exception& e )
    {
	// Whoops! An eval exception occurred. If it isn't a fatal one, 
	// inject a signal
	if ( !e.fatal()  )
	{    
	    _signal = sigref(new eval_signal_object( pContext, new eval_exception(e) ));
	    _signal->set_source_node(shared_from_this());
	}
	else
	    throw;
    }

    // Was a signal raised?
    if (_signal)
    {
	// Get the result from the signal handler instead
	result = _signal->handle(pContext,pContext->root_node(),_signal);

	// What if the signal couldn't be handled?
	if (!result)
	    throw eval_exception(cerror::unhandled_signal, "Unhandled signal", true );			     
    }

    return result;
}

void ast::compute_parent_map( map<astref,astref>& m )
{
    // Get the list of direct subordinates for this node
    list<astref> my_subordinates;
    direct_subordinates(my_subordinates);

    for ( auto node : my_subordinates )
    {
	// Set the direct subordinates to have 'this' as a parent
	m[node] = shared_from_this();

	// Repeat this operation for this subordinate
	node->compute_parent_map(m);
    }

}

void ast::set_observer(selectorref pSelNode)
{
    _observer = pSelNode;
}
