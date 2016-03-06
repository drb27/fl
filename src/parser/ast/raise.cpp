#include <string>
#include <set>
#include <list>
#include <vector>
#include <inc/references.h>
#include <parser/ast/raise.h>
#include <interpreter/obj/class_object.h>
#include <interpreter/obj/signal_object.h>
#include <interpreter/builtins.h>
#include <interpreter/factory.h>

using std::string;
using std::list;
using std::set;
using std::vector;

raise_node::raise_node(const astref& sigObjExpr)
    : _sigObjExpr(sigObjExpr)
{
}

bool raise_node::calls_and_returns( const string& fname) const
{
    return _sigObjExpr->calls_and_returns(fname);
}

void raise_node::render_dot(int& uuid, 
			     const string& parent,
			     const string& label,
			     std::ostream& out) const
{
    int myref=uuid++;
    string labelString;

    string myid = "raise_" + std::to_string(myref);
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    out << myid << "[shape=box" << labelString << "];" << std::endl;

    _sigObjExpr->render_dot(uuid,myid," sigcls",out);
}

void raise_node::required_symbols(set<symspec>& s) const
{
    _sigObjExpr->required_symbols(s);
}

objref raise_node::raw_evaluate(context* pContext)
{
    // Evaluate the signal object expression
    _signal = object::cast_or_abort<signal_object>( _sigObjExpr->evaluate(pContext) );
    _signal->set_source_node(shared_from_this());

    return objref(_signal);
}

fclass* raise_node::type(context* pContext) const
{
    return builtins::object::get_class();
}

asttype raise_node::type() const
{
    return asttype::raise;
}

void raise_node::direct_subordinates( list<astref>& subs ) const
{
    subs.push_back(_sigObjExpr);
}
