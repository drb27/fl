#include <string>
#include <vector>
#include <set>
#include <list>
#include <functional>
#include <inc/references.h>
#include <interpreter/marshall.h>
#include <parser/ast/attr.h>
#include <interpreter/eval_exception.h>
#include <interpreter/obj/class_object.h>

using std::string;
using std::set;
using std::list;
using std::vector;
using std::function;

attr_node::attr_node(ast* target, const string& selector)
    : _target(target), _selector(selector)
{

}

objref attr_node::evaluate(context* pContext)
{
    // Get the target
    objref target = _target->evaluate(pContext);

    // Check the attribute exists
    if ( target->has_attribute(_selector))
    {
	// Attribute exists
	return target->get_attribute(_selector);
    }
    else
    {
	// Attribute does not exist
	throw eval_exception(cerror::missing_attribute,"Could not find attribute " + _selector);
    }
}

void attr_node::required_symbols( set<string>& rs ) const
{
    _target->required_symbols(rs);
}

void attr_node::render_dot(int& uuid, 
			   const string& parent, 
			   const string& label, 
			   std::ostream& out) const
{
    int myref=uuid++;
    string labelString;

    string myid = "attr_" + _selector + "_" + std::to_string(myref);
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    out << myid << "[shape=box" << labelString << "];" << std::endl;
    _target->render_dot(uuid,myid," target",out);
}

function<void(objref)> attr_node::setter(context* pContext)
{
    return [this,pContext](objref pNewValue)
    {
	objref target = _target->evaluate(pContext);
	target->set_attribute(_selector,pNewValue);
    };
}

asttype attr_node::type() const
{
    return asttype::attr;
}

void attr_node::direct_subordinates( list<ast*>& subs ) const
{
    subs.push_back(_target);
}
