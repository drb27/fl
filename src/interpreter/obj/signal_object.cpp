#include <map>
#include <interpreter/obj/signal_object.h>
#include <interpreter/obj/class_object.h>
#include <interpreter/obj/string_object.h>
#include <parser/ast/selector.h>
#include <interpreter/context.h>

using std::map;
using std::string;

signal_object::signal_object(context* pContext,fclass& cls)
    : object(pContext,cls)
{
}

void signal_object::render( std::ostream& os, bool abbrev)
{
    object::render(os,abbrev);
}

void signal_object::set_source_node( const astref& n)
{
    _source_node=n;
}

objref signal_object::handle(context* pContext,const astref& rootNode,sigref pThis)
{
    // Compute the parent map for the root node
    map<astref,astref> parent;
    rootNode->compute_parent_map(parent);

    // Starting from the node which raised the signal, look for an upstream node which
    // advertises the ability to handle this signal
    astref currentNode = _source_node;
    bool more=true;

    while (more)
    {
	// Try to get an observer
	selectorref pSelNode(currentNode->observer());

	// Is there one?
	if (pSelNode)
	{
	    // Yes. Unwind the stack frame so we can evaluate it in context
	    pContext->unwind( pSelNode->unwind_stack_frame() );

	    // Evaluate the selector in the unwound context
	    try
	    {
		// Reference the signal in the current context
		pContext->assign( string("_signal"), pThis );

		auto result = pSelNode->evaluate(pContext);

		// Match found! Return the result
		return result;
	    }
	    catch ( eval_exception& )
	    {
		// Either the signal type didn't match, or some other error occurred. 
		// Fall through and try the next node up
	    }
	}

	auto i = parent.find(currentNode);

	if ( i==parent.end() )
	    more=false;
	else
	    currentNode=(*i).second;
    }    

    return objref(nullptr);
}

eval_signal_object::eval_signal_object(context* pContext, eval_exception* pEx, fclass& cls)
    : signal_object(pContext,cls), _exception(pEx)
{
    if (pEx)
	set_attribute("msg", stringref( new string_object(pContext, pEx->what() ) ) );
}

void eval_signal_object::set_exception( eval_exception* pEx )
{
    _exception = pEx;

    if (pEx)
	set_attribute("msg", stringref( new string_object(get_context(), pEx->what() ) ) );
}

eval_signal_object::~eval_signal_object()
{
    if (_exception)
	delete _exception;
}
