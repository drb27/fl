#include <string>
#include <set>
#include <list>
#include <inc/references.h>
#include <parser/ast/while.h>
#include <interpreter/obj/bool_object.h>
#include <interpreter/obj/void_object.h>

using std::string;
using std::list;
using std::set;

while_node::while_node(const astref& pCond, const astref& pAction)
    : _cond(pCond), _action(pAction)
{

}
void while_node::render_dot(int& uuid, 
			     const string& parent,
			     const string& label,
			     std::ostream& out) const
{
    int myref=uuid++;
    string labelString;

    string myid = "while_" + std::to_string(myref);
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    out << myid << "[shape=box" << labelString << "];" << std::endl;

    _cond->render_dot(uuid,myid," cond",out);
    _action->render_dot(uuid,myid," action",out);
}

#define BOOL_CAST(x) (object::cast_or_abort<bool_object>(x))->internal_value()

objref while_node::raw_evaluate(context* pContext )
{
    objref result( new void_object(pContext) );

    // Loop around executing the function until the condition is true
    while ( BOOL_CAST( _cond->evaluate(pContext) ) )
    {
	_action->evaluate(pContext);
    }
    
    return result;
}

void while_node::required_symbols(set<symspec>& s ) const
{
    _cond->required_symbols(s);
    _action->required_symbols(s);
}

asttype while_node::type() const
{
    return asttype::_while;
}

void while_node::direct_subordinates( list<astref>& subs ) const
{
    subs.push_back(_cond);
    subs.push_back(_action);
}
