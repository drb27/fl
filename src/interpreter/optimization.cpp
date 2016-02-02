#include <string>
#include <list>
#include "optimization.h"
#include <parser/ast_nodes.h>
#include <interpreter/eval_exception.h>
#include <logger/logger.h>

using std::list;

namespace opt
{

    optimization::optimization()
    {
    }

    optimization::~optimization()
    {

    }
    
    ast* optimization::search( ast* pRootNode) const
    {
	pattern ptn;
	get_pattern(ptn);
	ptn.index=0;
	ptn.matchedHead = nullptr;

	// Check the anchor
	if (ptn.anchor && (pRootNode->type() != ptn.typestack[0]) )
	    return nullptr;

	return recursive_search(pRootNode, ptn);
    }
    
    ast* optimization::recursive_search( ast* pNode, pattern p ) const
    {
	// Which mode are we in?
	if (p.matchedHead)
	{
	    // We're in the middle of a pattern match. Does this node match?
	    if ( pNode->type() == p.typestack[p.index] )
	    {
		// Yep. Is it the last node in the pattern?
		if ( p.index >= (p.typestack.size()-1) )
		{
		    // Yes - we have a complete match!
		    return p.matchedHead;
		}
		else
		{
		    // No - continue the search
		    list<ast*> subordinates;
		    pNode->direct_subordinates(subordinates);
		    p.index++;
		    for ( auto s : subordinates )
		    {
			ast* result = recursive_search(s,p);
			if (result) return result;
		    }
		}
	    }
	    else
	    {
		// Nope - we're done.
		return nullptr;
	    }
	}
	else
	{
	    // We are looking for the head. Is this it?
	    if ( pNode->type() == p.typestack[p.index] )
	    {
		// Yes, this is the head. Remember where we are and
		// attempt to match the rest of the pattern.
		p.matchedHead = pNode;
		p.index++;
		
		list<ast*> subordinates;
		pNode->direct_subordinates(subordinates);
		for ( auto s : subordinates )
		{
		    ast* result = recursive_search(s,p);
		    if (result) return result;
		}
	    }
	    else
	    {
		// No, this is not the head. Try the subordinates
		list<ast*> subordinates;
		pNode->direct_subordinates(subordinates);
		for ( auto s : subordinates )
		{
		    ast* result = recursive_search(s,p);
		    if (result) return result;
		}
	    }
	}

	// No match
	return nullptr;
    }

    bool optimization::search_and_destroy(ast* pRootNode) const
    {
	ast* result=nullptr;
	bool found=false;
	do
	{
	    result = search(pRootNode);
	    if (result)
	    {
		found=true;
		execute(result);
	    }
	} while (result);

	return found;
    }

    void if_tailcall::get_pattern( pattern& p ) const
    {
	p.anchor=true;
	p.typestack.push_back( asttype::fundef );
	p.typestack.push_back( asttype::_if );
	p.typestack.push_back( asttype::funcall);
	p.index=0;
	p.matchedHead = nullptr;
    }

    bool if_tailcall::execute(ast* pHeadNode) const
    {
	wlog_entry();
	fundef_node* pFunDef = dynamic_cast<fundef_node*>(pHeadNode);
	list_node* pArgs = pFunDef->args();
	if_node* pIf = dynamic_cast<if_node*>(pFunDef->def());
	ast* pCondition = pIf->cond();

	ast *pReturn;
	list_node *pCallList=nullptr;
	funcall_node *pFunCall;

	bool callIsOnTrue;

	if ( ( pIf->true_expr()->type() == asttype::funcall ) &&  
	     ( dynamic_cast<funcall_node*>(pIf->true_expr())->name() == _name )
	     )
	{
	    callIsOnTrue = true;
	    pReturn = pIf->false_expr();
	    pFunCall = dynamic_cast<funcall_node*>(pIf->true_expr());
	}
	else if (( pIf->false_expr()->type() == asttype::funcall ) &&   
		 ( dynamic_cast<funcall_node*>(pIf->false_expr())->name() == _name ) )
	{
	    callIsOnTrue = false;
	    pReturn = pIf->true_expr();
	    pFunCall = dynamic_cast<funcall_node*>(pIf->false_expr());
	}
	else
	    return false;

	// Parameter list for subsequent calls
	if (pFunCall->args()->type() == asttype::list )
	    pCallList = dynamic_cast<list_node*>(pFunCall->args());

	// TODO: Construct the translated tree
	sequence_node* pParamUpdates = new sequence_node(true);

	if (pCallList)
	{
	    // Construct assignments for the param update sequence
	    list<ast*> elements = pCallList->raw_elements();
	    auto basicParamIterator = pArgs->raw_elements().begin();
	    for ( auto e : elements )
	    {
		assign_node* pAssign = new assign_node(*basicParamIterator,e,true);
		basicParamIterator++;
		pParamUpdates->add_expr(pAssign);
	    }
	}
	else
	{
	    throw eval_exception(cerror::internal_error, "Unhandled condition in optimization::execute");
	    // Just null
	    // TODO
	}

	if (!callIsOnTrue)
	{
	    auto pOldCond = pCondition;
	    pCondition = new methodcall_node("not");
	    dynamic_cast<methodcall_node*>(pCondition)->add_target(pOldCond);
	}
	
	while_node* pWhile = new while_node( pCondition, pParamUpdates );

	sequence_node* pTopSequence = new sequence_node();
	pTopSequence->add_expr(pWhile);
	pTopSequence->add_expr(pReturn);

	// Update the definition of the function to the new tree
	pFunDef->replace_definition(pTopSequence);
	wlog(level::info, "Successfully eliminated tail recursion");

	return true;
    }
}
