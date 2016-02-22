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

raise_node::raise_node(ast* classExpr)
    : _classExpr(classExpr)
{
}

bool raise_node::calls_and_returns( const string& fname) const
{
    return _classExpr->calls_and_returns(fname);
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

    _classExpr->render_dot(uuid,myid," sigcls",out);
}

void raise_node::required_symbols(set<symspec>& s) const
{
    _classExpr->required_symbols(s);
}

objref raise_node::raw_evaluate(context* pContext)
{
    // Evaluate the class expression
    classref pCls = object::cast_or_abort<class_object>( _classExpr->evaluate(pContext) );

    if (!pCls)
	throw eval_exception(cerror::not_a_class,
			     "Argument to raise expression did not evaluate to a class object" );

    // TODO: Check that class is a signal class

    // TODO: Support parameters to constructor
    vector<ast*> params;
    _signal = object::cast_or_abort<signal_object>(factory::get().make_object(pContext,
									      pCls->internal_value(),
									      params) );
    _signal->set_source_node(this);

    return objref(nullptr);
}

fclass* raise_node::type(context* pContext) const
{
    return builtins::object::get_class();
}

asttype raise_node::type() const
{
    return asttype::raise;
}

void raise_node::direct_subordinates( list<ast*>& subs ) const
{
    subs.push_back(_classExpr);
}
