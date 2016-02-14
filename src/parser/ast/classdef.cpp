#include <string>
#include <set>
#include <list>
#include <inc/references.h>
#include <parser/ast/classdef.h>
#include <interpreter/eval_exception.h>
#include <interpreter/obj/list_object.h>
#include <interpreter/obj/class_object.h>
#include <interpreter/context.h>

using std::string;
using std::list;
using std::set;

classdef_node::classdef_node(const string& name, ast* pDeriveList)
    :_derive_list(pDeriveList), _name(name)
{
}

objref classdef_node::evaluate(context* pContext)
{
    // Ensure that exactly one parameter has been given
    listref paramList = 
	std::dynamic_pointer_cast<list_object>(_derive_list->evaluate(pContext));
    
    if (paramList->size()!=1)
	throw eval_exception( cerror::no_multiple_inheritance,
			      "fl does not support multiple inheritance" );

    // Ensure that the passed parameter is in fact a class object
    if ( &(paramList->get_element(0)->get_class()) != builtins::flclass::get_class() )
	throw eval_exception( cerror::unsupported_argument,
			      "Argument to defclass must be a class object" );

    // Ensure that the target class is not sealed
    classref pBaseClass = std::dynamic_pointer_cast<class_object>(paramList->get_element(0));

    if (pBaseClass->is_sealed() )
	throw eval_exception( cerror::sealed_class,
			      "Cannot derive a new class from a sealed class" );

    // Checks complete - create the new class & register with the type manager
    typespec ts(_name);
    fclass* pNewClass = new fclass(ts,pBaseClass->internal_value() );
    pContext->types()->add(*pNewClass);

    // Create a class object and add it to the current context
    classref pNewClassObject = classref( new class_object(pContext, pNewClass) );
    pContext->assign(_name,pNewClassObject);

    // Return the new class object
    return pNewClassObject;
}

void classdef_node::required_symbols(set<string>& s) const
{
    _derive_list->required_symbols(s);
}

void classdef_node::render_dot(int& uuid,
			   const string& parent, 
			   const string& label,
			   std::ostream& out) const
{
    int myref=uuid++;
    string labelString;

    string myid = "classdef_" + _name + "_" + std::to_string(myref);
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    out << myid << "[shape=box" << labelString << "];" << std::endl;

    _derive_list->render_dot(uuid,myid," cond",out);
}

asttype classdef_node::type() const
{
    return asttype::classdef;
}

void classdef_node::direct_subordinates( list<ast*>& s) const
{
    s.push_back(_derive_list);
}
