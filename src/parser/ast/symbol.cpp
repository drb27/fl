#include <string>
#include <set>
#include <functional>
#include <list>
#include <inc/references.h>
#include <interpreter/marshall.h>
#include <parser/ast/symbol.h>
#include <interpreter/eval_exception.h>
#include <interpreter/optimization.h>
#include <logger/logger.h>
#include <interpreter/obj/fn_object.h>
#include <interpreter/context.h>

using std::string;
using std::set;
using std::function;
using std::list;

symbol_node::symbol_node(const string& name)
    : _name(name)
{

}

function<void(objref)> symbol_node::setter(context* pContext)
{
    return [pContext,this](objref pNewVal)
    {

	// Set function name, if this is a function
	if ( builtins::function::get_class()==&(pNewVal->get_class()) )
	{
	    // Yep!
	    fnref pFn = std::dynamic_pointer_cast<fn_object>(pNewVal);

	    if ( pFn->is_anonymous() && !pFn->raw().is_builtin() )
	    {
		pFn->set_name(_name);

		ast* head;
		opt::if_tailcall o(_name);
		if ( head = o.search( pFn->raw().def() ) )
		{
		    if ( o.execute(head) )
		    {
			pFn->raw().regenerate_function(pFn->raw().def());
			wlog(level::info,"Optmized " + _name);
		    }
		}
	    }
	}

	pContext->assign(symspec(_name,_pkg_spec),pNewVal);

    };
}

void symbol_node::render_dot(int& uuid, 
			     const string& parent, 
			     const string& label,
			     std::ostream& out) const
{
    int myref=uuid++;
    string labelString;

    string myid = "symbol_" + _name + "_" + std::to_string(myref);
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    out << myid << "[shape=box" << labelString << "];" << std::endl;
}

const string& symbol_node::name() const
{
    return _name;
}

const list<string> symbol_node::pkg_spec() const
{
    return _pkg_spec;
}

void symbol_node::required_symbols(set<symspec>& s) const
{
    s.insert(_name);
}

objref symbol_node::evaluate(context* pContext)
{
    return pContext->resolve_symbol( symspec(_name,_pkg_spec) ); 
}


fclass* symbol_node::type(context* pContext) const
{
    // Attempt to resolve the symbol
    auto o = pContext->resolve_symbol(_name);

    if (o)
	return &(o->get_class());

    throw eval_exception(cerror::internal_error,
			 "Unable to resolve symbol_node(_name) in symbol_node::type()");
}

asttype symbol_node::type() const
{
    return asttype::symbol;
}

void symbol_node::direct_subordinates( list<ast*>& subs ) const
{
}
void symbol_node::add_pkg_spec( const list<string>& spec)
{
    _pkg_spec = spec;
}

symspec symbol_node::sym_spec() const
{
    return symspec(_name,_pkg_spec);
}
