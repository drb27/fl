#include <string>
#include <vector>
#include <set>
#include <list>
#include <functional>
#include <inc/references.h>
#include <interpreter/marshall.h>
#include <parser/ast/methodcall.h>
#include <interpreter/eval_exception.h>
#include <interpreter/context.h>
#include <interpreter/obj/class_object.h>

using std::string;
using std::set;
using std::list;
using std::vector;
using std::function;

methodcall_node::methodcall_node(const string& name)
    : _name(name)
{

}

void methodcall_node::render_dot(int& uuid, 
				 const string& parent, 
				 const string& label,
				 std::ostream& out) const
{
    int myref=uuid++;
    string labelString;

    string myid = "methodcall_" + _name + "_" + std::to_string(myref);
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    out << myid << "[shape=box" << labelString << "];" << std::endl;
    _target->render_dot(uuid,myid," target",out);

    int paramCount=0;
    for (auto arg : _params)
    {
	auto pc = paramCount++;
	arg->render_dot(uuid,myid," param" + std::to_string(pc),out);
    }
}

objref methodcall_node::evaluate(context* pContext)
{
    state_guard g(pContext);
    // Evaluate the target
    objref target = _target->evaluate(pContext);

    // Look up the method on the class
    function<marshall_mthd_t> m; 

    if (target->get_class().has_method(_name) )
	m = target->get_class().lookup_method(_name).fn;
    else
    {

	if ( &(target->get_class()) == builtins::flclass::get_class() )
    	{
    	    classref targetCls = std::dynamic_pointer_cast<class_object>(target);
    	    if ( targetCls->internal_value()->has_class_method(_name) )
		m = targetCls->internal_value()->lookup_class_method(_name).fn;
    	}
    }
    
    if (!m)
	throw eval_exception(cerror::undefined_method,"Undefined method " + _name);

    // Prepare the parameter vector
    auto params = vector<ast*>(_params.size()+2);

    int index=2;
    for ( auto p : _params )
    {
	params[index++] = p;
    }

    g.new_collection();

    // Dispatch the call
    auto retVal =  m(pContext,target,params);

    // Return result
    return retVal;

}

void methodcall_node::required_symbols(set<string>& s) const
{
    _target->required_symbols(s);
    for (auto p : _params)
    {
	p->required_symbols(s);
    }
}

void methodcall_node::add_target(ast* pObj)
{
    _target = pObj;
}

void methodcall_node::add_param(ast* pNode)
{
    _params.push_back(pNode);
}

void methodcall_node::direct_subordinates( list<ast*>& subs ) const
{
    subs.push_back(_target);
    for ( auto n : _params )
	subs.push_back(n);
}

asttype methodcall_node::type() const
{
    return asttype::methodcall;
}
