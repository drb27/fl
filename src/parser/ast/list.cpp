#include <string>
#include <set>
#include <list>
#include <inc/references.h>
#include <interpreter/obj/class_object.h>
#include <interpreter/obj/list_object.h>
#include <parser/ast/list.h>
#include <interpreter/eval_exception.h>

using std::string;
using std::set;
using std::list;

list_node::list_node( std::initializer_list<ast*> l ) : list_node(false)
{
    for ( auto el : l )
	push_element(el);
}

void list_node::push_element(ast* pNode)
{
    _elements.push_back(pNode);
}

void list_node::render_dot(int& uuid, 
			     const string& parent,
			     const string& label,
			     std::ostream& out) const
{
    int myref=uuid++;
    string labelString;

    string myid = "list_" + std::to_string(myref);
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    out << myid << "[shape=box" << labelString << "];" << std::endl;

    int paramCount=0;
    for (auto arg : _elements)
    {
	auto pc = paramCount++;
	arg->render_dot(uuid,myid," el" + std::to_string(pc),out);
    }
}

objref list_node::raw_evaluate(context* pContext)
{
    // Make a list of evaluated elements
    list<objref> items;
    auto class_cls = builtins::flclass::get_class();

    for ( auto e : _elements )
    {
	ast* pTypeHintNode = e->get_typehint();
	if (pTypeHintNode)
	{
	    // Evaluate the type hint in the context
	    objref pHintClass = pTypeHintNode->evaluate(pContext);
	    // Ensure this is of type 'class'
	    if ( &(pHintClass->get_class()) == class_cls )
	    {
		classref pHintClassAsClass = std::dynamic_pointer_cast<class_object>(pHintClass);
		items.push_back(object::convert_to(e->evaluate(pContext),
						   pHintClassAsClass->internal_value() ));
	    }
	    else
	    {
		throw eval_exception(cerror::not_a_class,
				     "Type hint does not evaluate to a class" );
	    }
	}
	else
	{
	    items.push_back(e->evaluate(pContext));
	}
    }

    auto l = new list_object(pContext,items);
    return objref(l);
}

void list_node::required_symbols(set<symspec>& s) const
{
    for ( auto e : _elements)
    {
	e->required_symbols(s);
    }
}

list<ast*>& list_node::raw_elements()
{
    return _elements;
}

fclass* list_node::type(context* pContext) const
{
    return builtins::list::get_class();    
}

asttype list_node::type() const
{
    return asttype::list;
}

void list_node::direct_subordinates( list<ast*>& subs ) const
{
    for ( auto e : _elements)
	subs.push_back(e);
}
