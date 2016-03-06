#include <string>
#include <set>
#include <list>
#include <deque>
#include <algorithm>
#include <inc/references.h>
#include <parser/ast/fundef.h>
#include <parser/ast/symbol.h>
#include <interpreter/obj/fn_object.h>
#include <logger/logger.h>
#include <interpreter/context.h>

using std::string;
using std::list;
using std::set;
using std::deque;
using std::pair;
using std::find_if;
using std::for_each;

fundef_node::fundef_node(const astref& arglist, const astref& definition)
    : _arglist(arglist), _definition(definition)
{
    wlog_entry();
    wlog_exit();
}

bool fundef_node::is_tail_recursive(const string& myName) const
{
    // TODO
    return calls_and_returns(myName);
}

void fundef_node::render_dot(int& uuid, 
			     const string& parent,
			     const string& label,
			     std::ostream& out) const
{
    int myref=uuid++;
    string labelString;
   
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
   
    string myid = "fundef_" + std::to_string(myref);
    out << myid << "[shape=box" << labelString <<  "];" << std::endl;
    _arglist->render_dot(uuid,myid," args",out);
    _definition->render_dot(uuid,myid," def",out);

}

void fundef_node::required_symbols(set<symspec>& s) const
{
    // Definitions which are not arguments
    set<symspec> tempSet;
    set<string> args;
    set<symspec> argSpecs;

    // Get all symbols used by the function body
    _definition->required_symbols(tempSet);

    // Get all arguments provided by the function caller
    _arglist->required_symbols(argSpecs);

    std::for_each( argSpecs.begin(), argSpecs.end(), 
		   [&args](const symspec& s)
		   {
		       args.insert(s.name());
		   } 
		   );

    // Subtract arguments from the definition set.
    // This uses find_if to locate required symbols with the same name as an argument, AND
    // having no package specifier. This is guaranteed to be a reference to the argument,
    // as the unscoped reference binds most closely to the argument, even if another
    // matching symbol outside of the function definition exists. 
    for ( auto arg : args)
    {
	auto it = find_if( tempSet.begin(),
			   tempSet.end(),
			   [&arg]( const symspec& s) 
			   { 
			       return (s.name()==arg) && (s.pkg_spec().empty()); 
			   } );

	if (it != tempSet.end() )
	    tempSet.erase(it);
    }

    // Add the remaining non-argument symbols to the result
    for ( auto sym : tempSet )
    {
	s.insert(sym);
    }
    
}


objref fundef_node::raw_evaluate(context* pContext)
{
    wlog_entry();

    set<symspec> requiredSymbols;
    required_symbols(requiredSymbols);

    for ( auto s : requiredSymbols )
    {
	wlog(level::debug, "Required symbol: " + s.rqn() );
    }

    deque<pair<string,astref>> argnames;

    auto pArgList(args());
    for ( auto sn : pArgList->raw_elements() )
    {
	symnoderef pSymNode = std::dynamic_pointer_cast<symbol_node>(sn);
	argnames.push_back({pSymNode->name(),pSymNode->get_typehint()});
    }

    colref pClosure( new closure_collection );
    for ( auto s : requiredSymbols )
    {
	if (pContext->is_defined(s))
	    pClosure->define_symbol(s, pContext->resolve_symbol(s) );
    }

    return objref( new fn_object(pContext,
				 std::dynamic_pointer_cast<fundef_node>(shared_from_this()),
				 pClosure,
				 argnames,argnames,{}) );
}

fclass* fundef_node::type(context* pContext) const
{
    return builtins::function::get_class();
}

asttype fundef_node::type() const
{
    return asttype::fundef;
}

void fundef_node::direct_subordinates( list<astref>& subs ) const
{
    subs.push_back(_arglist);
    subs.push_back(_definition);
}
