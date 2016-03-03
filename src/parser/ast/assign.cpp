#include <string>
#include <set>
#include <list>
#include <vector>
#include <inc/references.h>
#include <parser/ast/lvalue.h>
#include <parser/ast/assign.h>
#include <interpreter/eval_exception.h>
#include <interpreter/obj/class_object.h>

using std::string;
using std::list;
using std::set;
using std::vector;

assign_node::assign_node(const astref& lvalue, const astref& rvalue, bool alias)
    : _lvalue(lvalue),_rvalue(rvalue),_alias(alias)
{

}

void assign_node::render_dot(int& uuid, 
			     const string& parent,
			     const string& label,
			     std::ostream& out) const
{
    int myref=uuid++;
    string labelString;

    string myid = "assign_" + std::to_string(myref);
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    out << myid << "[shape=box" << labelString << "];" << std::endl;

    _lvalue->render_dot(uuid,myid," lvalue",out);
    _rvalue->render_dot(uuid,myid," rvalue",out);
}

void assign_node::required_symbols(set<symspec>& s) const
{
    _lvalue->required_symbols(s);
    _rvalue->required_symbols(s);
}

objref assign_node::raw_evaluate(context* pContext)
{
    // evaluate RHS and LHS

    auto rhs = _rvalue->evaluate(pContext);
    rhs->optimise();

    if (_alias)
    {
	if ( _lvalue->is_lvalue())
	{
	    lvalue_node* pLvalue = dynamic_cast<lvalue_node*>(_lvalue.get());
	    auto setfn = pLvalue->setter(pContext);
	    setfn(rhs);
	    auto lhs = _lvalue->evaluate(pContext);
	    return lhs;
	}
	else
	{
	    throw eval_exception(cerror::invalid_lvalue,"Invalid lvalue");
	}
    }
    else
    {
	auto lhs = _lvalue->evaluate(pContext);
	// Call the assign method on the lhs
	methodinfo m = lhs->get_class().lookup_method(".assign");
	vector<ast*> params(3);
	params[2] = _rvalue.get();
	m.fn(pContext,lhs,params);
	return lhs;
    }

}

fclass* assign_node::type(context* c) const
{
    return _lvalue->type(c);
}

asttype assign_node::type() const
{
    return asttype::assign;
}

void assign_node::direct_subordinates( list<astref>& subs ) const
{
    subs.push_back(_lvalue);
    subs.push_back(_rvalue);
}
