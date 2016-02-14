#include <string>
#include <set>
#include <list>
#include <deque>
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

fundef_node::fundef_node(ast* arglist, ast* definition)
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

void fundef_node::required_symbols(set<string>& s) const
{
    // Definitions which are not arguments
    set<string> tempSet;
    set<string> args;

    // Get all used symbols
    _definition->required_symbols(tempSet);

    // Get argument symbols
    _arglist->required_symbols(args);

    // Subtract arguments from the definition set
    for ( auto arg : args)
    {
	tempSet.erase(arg);
    }

    // Add the remaining non-argument symbols to the result
    for ( auto sym : tempSet )
    {
	s.insert(sym);
    }
    
}


objref fundef_node::evaluate(context* pContext)
{
    wlog_entry();

    set<string> requiredSymbols;
    required_symbols(requiredSymbols);

    for ( auto s : requiredSymbols )
    {
	wlog(level::debug, "Required symbol: " + s );
    }

    deque<pair<string,ast*>> argnames;

    list_node* pArgList = dynamic_cast<list_node*>(_arglist);
    for ( auto sn : pArgList->raw_elements() )
    {
	symbol_node* pSymNode = dynamic_cast<symbol_node*>(sn);
	argnames.push_back({pSymNode->name(),pSymNode->get_typehint()});
    }

    colref pClosure( new collection );
    for ( auto s : requiredSymbols )
    {
	if (pContext->is_defined(s))
	    (*pClosure)[s] = pContext->resolve_symbol(s);
    }

    return objref( new fn_object(pContext,rawfn(this,pClosure),argnames,{}) );
}

fclass* fundef_node::type(context* pContext) const
{
    return builtins::function::get_class();
}

asttype fundef_node::type() const
{
    return asttype::fundef;
}

void fundef_node::direct_subordinates( list<ast*>& subs ) const
{
    subs.push_back(_arglist);
    subs.push_back(_definition);
}
