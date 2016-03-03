#include <string>
#include <vector>
#include <set>
#include <list>
#include <functional>
#include <inc/references.h>
#include <interpreter/marshall.h>
#include <parser/ast/methodcall.h>
#include <parser/ast/literal.h>
#include <interpreter/eval_exception.h>
#include <interpreter/context.h>
#include <interpreter/obj/class_object.h>
#include <interpreter/obj/list_object.h>

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
    _param_list->render_dot(uuid,myid," params",out);
}

objref methodcall_node::raw_evaluate(context* pContext)
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

    // Evaluate the params list
    listref pEvaledParams;

    if (_param_list)
	pEvaledParams = object::cast_or_abort<list_object>( _param_list->evaluate(pContext) );
    else
	pEvaledParams = listref(new list_object(pContext));

    // Prepare the parameter vector
    auto params = vector<ast*>(pEvaledParams->size()+2);

    int index=2;
    for (int pidx=0; pidx<pEvaledParams->size(); pidx++)
    {
	params[index++] = new literal_node(pEvaledParams->get_element(pidx));
    }

    pContext->new_collection();

    // Dispatch the call
    auto retVal =  m(pContext,target,params);

    // Delete the new literal_nodes we created
    for( index=2; index< params.size(); index++ )
	delete params[index];

    // Return result
    return retVal;

}

void methodcall_node::required_symbols(set<symspec>& s) const
{
    _target->required_symbols(s);
    _param_list->required_symbols(s);
}

void methodcall_node::add_target(const astref& pObj)
{
    _target = pObj;
}

void methodcall_node::add_param_list(const astref& pNode)
{
    _param_list = pNode;
}

void methodcall_node::direct_subordinates( list<astref>& subs ) const
{
    subs.push_back(_target);
    subs.push_back(_param_list);
}

asttype methodcall_node::type() const
{
    return asttype::methodcall;
}
