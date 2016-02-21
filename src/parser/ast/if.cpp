#include <string>
#include <set>
#include <list>
#include <inc/references.h>
#include <parser/ast/if.h>
#include <interpreter/obj/bool_object.h>

using std::string;
using std::list;
using std::set;

if_node::if_node(ast* pCondition, ast* trueExpression, ast* falseExpression)
    : _condition(pCondition), _trueExpr(trueExpression), _falseExpr(falseExpression)
{
}

bool if_node::calls_and_returns( const string& fname) const
{
    // True if either the true branch or false branch returns true
    return ( _trueExpr->calls_and_returns(fname) || _falseExpr->calls_and_returns(fname) );
}

void if_node::render_dot(int& uuid, 
			     const string& parent,
			     const string& label,
			     std::ostream& out) const
{
    int myref=uuid++;
    string labelString;

    string myid = "if_" + std::to_string(myref);
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    out << myid << "[shape=box" << labelString << "];" << std::endl;

    _condition->render_dot(uuid,myid," cond",out);
    _trueExpr->render_dot(uuid,myid," true",out);
    _falseExpr->render_dot(uuid,myid," false",out);
}

void if_node::required_symbols(set<symspec>& s) const
{
    _condition->required_symbols(s);
    _trueExpr->required_symbols(s);
    _falseExpr->required_symbols(s);
}

objref if_node::evaluate(context* pContext)
{
    boolref cond = object::cast_or_abort<bool_object>(_condition->evaluate(pContext));
    
    if (cond->internal_value())
	return _trueExpr->evaluate(pContext);
    else
	return _falseExpr->evaluate(pContext);
}

fclass* if_node::type(context* pContext) const
{
    return builtins::object::get_class();
}

asttype if_node::type() const
{
    return asttype::_if;
}

void if_node::direct_subordinates( list<ast*>& subs ) const
{
    subs.push_back(_condition);
    subs.push_back(_trueExpr);
    subs.push_back(_falseExpr);
}
