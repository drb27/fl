#include <parser/ast/range.h>
#include <parser/ast/list.h>
#include <parser/ast/literal.h>
#include <parser/ast/methodcall.h>
#include <interpreter/obj/class_object.h>

using std::string;
using std::list;
using std::set;

range_node::range_node(const astref& f, const astref& s )
    : pair_node(f,s)
{
}

objref range_node::raw_evaluate(context* pContext)
{
    // Evaluate both range bounds
    objref pStart = _first->evaluate(pContext);
    objref pEnd = _second->evaluate(pContext);

    // Get a class object for the first object
    objref pClass = objref( new class_object(pContext,&(pStart->get_class())) );

    // Construct a parameter list
    auto pParamList = listnoderef( new list_node() );
    pParamList->push_element( astref( new literal_node(pStart) ));
    pParamList->push_element( astref( new literal_node(pEnd) ));
    
    // Execute the range class method on the class of the first object
    auto pCall = methodcallref( new methodcall_node("range"));
    pCall->add_target( astref( new literal_node(pClass) ) );
    pCall->add_param_list(pParamList);

    return pCall->evaluate(pContext);
}

asttype range_node::type() const
{
    return asttype::range;
}
