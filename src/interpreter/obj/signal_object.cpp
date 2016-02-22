#include <map>
#include <interpreter/obj/signal_object.h>
#include <interpreter/obj/class_object.h>
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

void signal_object::set_source_node( ast* n)
{
    _source_node=n;
}

objref signal_object::handle(context* pContext,ast* rootNode)
{
    // Compute the parent map for the root node
    map<ast*,ast*> parent;
    rootNode->compute_parent_map(parent);

    // Starting from the node which raised the signal, look for an upstream node which
    // advertises the ability to handle this signal
    ast* currentNode = _source_node;
    bool found=false;

    while (!found)
    {
	// Try to get an observer
	selector_node* pSelNode = currentNode->observer();

	// Is there one?
	if (pSelNode)
	{
	    // Yes. Unwind the stack frame so we can evaluate it in context
	    pContext->unwind( pSelNode->unwind_stack_frame() );

	    // Evaluate the selector in the unwound context
	    try
	    {
		// Reference the signal in the current context
		pContext->assign( string(".signal"),
				  objref( new class_object(pContext,
							   &get_class()) ) 
				  );

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

	currentNode = parent[currentNode];

	// TODO: Termination of this loop when no handler is successful
	
    }
    

    return objref(nullptr);
}
