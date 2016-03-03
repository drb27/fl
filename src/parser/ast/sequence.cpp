#include <string>
#include <set>
#include <list>
#include <inc/references.h>
#include <parser/ast/sequence.h>
#include <interpreter/context.h>
#include <interpreter/collection.h>

using std::string;
using std::list;
using std::set;

sequence_node::sequence_node(bool isolated)
    :_isolated(isolated)
{

}

void sequence_node::render_dot(int& uuid, 
			     const string& parent,
			     const string& label,
			     std::ostream& out) const
{
    int myref=uuid++;
    string labelString;

    string myid = "seq_" + std::to_string(myref);
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    out << myid << "[shape=box" << labelString << "];" << std::endl;

    int iSeq=0;
    for ( auto e : _sequence )
    {
	e->render_dot(uuid,myid," s" + std::to_string(iSeq++),out);
    }
}

bool sequence_node::calls_and_returns( const string& fname) const
{
    // True if the last expression in the sequence returns true
    return _sequence.back()->calls_and_returns( fname );
}

objref sequence_node::raw_evaluate(context* pContext)
{
    if (_isolated)
	return evaluate_isolated(pContext);

    objref latestResult;
    for ( auto expr : _sequence )
    {
	latestResult=expr->evaluate(pContext);
    }

    return latestResult;
}

objref sequence_node::evaluate_isolated(context* pContext)
{
    // TODO: The following use of context is not exception-safe!

    objref latestResult;
    list<colref> callCollections;
    for ( auto expr : _sequence )
    {
	// Create a new collection for this call only
	pContext->new_collection();

	// Execute the expression in this private context
	latestResult = expr->evaluate(pContext);

	// Pop the collection off the stack
	callCollections.push_back( pContext->pop_collection() );
    }

    // Merge all of the new collections, in order from oldest to newest
    colref currentCollection = pContext->current_collection();
    for ( auto col : callCollections )
    {
	for ( auto p : *(col.get()) )
	{
	    currentCollection->define_symbol(p.first,p.second);
	}
    }

    // Return the result of the last evaluation
    return latestResult;
}

void sequence_node::required_symbols(set<symspec>& s) const
{
    for ( auto expr : _sequence )
    {
	expr->required_symbols(s);
    }
}

void sequence_node::add_expr(const astref& expr)
{
    _sequence.push_back(expr);
}

fclass* sequence_node::type(context* pContext) const
{
    return _sequence.back()->type(pContext);
}

asttype sequence_node::type() const
{
    return asttype::sequence;
}

void sequence_node::direct_subordinates( list<astref>& subs ) const
{
    for ( auto n : _sequence)
	subs.push_back(n);
}
