#include <deque>
#include <set>
#include <interpreter/obj/object.h>
#include <interpreter/obj/void_object.h>
#include <interpreter/obj/string_object.h>
#include <interpreter/eval_exception.h>
#include <logger/logger.h>
#include <parser/ast/methodcall.h>
#include <parser/ast/literal.h>
#include <parser/ast/list.h>

using std::deque;
using std::string;
using std::vector;
using std::set;

object::object(context* pContext, fclass& c, vector<objref> params) 
    : _class(c),_context(pContext)
{
    if ( c.is_abstract() )
	throw eval_exception(cerror::instantiate_abstract,
			     "An attempt was made to instantiate an object of an abstract class");

    // Create the attributes for this class, and the base chain
    fclass* pCurrentClass = &c;

    auto fn = [this,pContext](fclass* pClass)
	{
	    for ( auto a : pClass->attributes() )
	    {
		objref value = a.second;
		_attributes[a.first] = (value)? value : objref( new void_object(pContext) );
	    }
	};

    for ( auto pCls : c.upstream_hierarchy() )
	fn(pCls);

}

objref object::convert_to(objref pThis, fclass* pOther)
{
    // Are we already the right type?
    if (pOther==&(pThis->get_class()))
	return pThis;

    // ... or already a decendant of pOther?
    if ( pThis->get_class().is_a(*pOther) )
	return pThis;

    set<ctnoderef> solutionSet;
    bool canConvert = pThis->get_class().build_conversion_tree(pOther,solutionSet);

    if (!canConvert)
    {
	throw eval_exception(cerror::unsupported_conversion,"Conversion from class " 
			     + pThis->get_class().name() + " to class " 
			     + pOther->name() + " is not supported");
    }

    // Select the shortest solution (fewest number of conversion calls)
    int shortest=0;
    ctnoderef solution;

    for ( auto sln : solutionSet )
    {
	if (!shortest)
	{
	    shortest=sln->length();
	    solution = sln;
	}
	else
	{
	    int thisLength = sln->length();
	    if ( thisLength<shortest)
	    {
		shortest = thisLength;
		solution = sln;
	    }
	}
    }

    // Now solution points to the shortest solution. Built a list of method
    // names to call

    deque<string> methodChain;
    while (solution)
    {
	methodChain.push_front( "->" + solution->cls->name() );
	solution = solution->parent;
    }

    // Remove the first call, it is unnecessary
    methodChain.pop_front();

    // Call each method on ourselves
    objref currentObject = pThis;
    vector<objref> params;
    for (auto mth : methodChain)
    {
	currentObject = currentObject->invoke(mth,pThis->get_context(),params);
    }

    return currentObject;
}

bool object::operator==(const objref other) const
{
    return false;
}

bool object::selector_match( const objref other) const
{
    return (*this)==other;
}

object& object::operator=(const objref other)
{
    throw eval_exception(cerror::invalid_assignment, "Incompatible assignment");
}

object::~object()
{
    wlog_entry();
}

objref object::invoke( const string& mthdName, context* pContext, vector<objref>& params)
{
    // Create a methodcall node to do all of the hard work for us
    methodcall_node* pCallNode = new methodcall_node(mthdName);

    // Set up the node here
    pCallNode->add_target( astref(new literal_node( objref(this,[](object*){}) )));

    auto pArgList = new list_node();

    for ( auto param : params )
    {
	pArgList->push_element( astref(new literal_node( param ) ) );
    }

    pCallNode->add_param_list(astref(pArgList));

    objref result = pCallNode->evaluate(pContext);

    delete pCallNode;

    return result;
}

bool object::has_attribute(const std::string& name) const
{
    return _attributes.find(name)!=_attributes.end();
}

bool object::has_method(const std::string& name ) const
{
    return get_class().has_method(name);
}
    
objref object::get_attribute(const std::string& selector)
{
    return _attributes[selector];
}

void object::create_attribute(const std::string& selector)
{
    voidref o = voidref( new void_object(get_context()) );
    _attributes[selector] = o;
}

void object::set_attribute(const std::string& selector, objref newValue)
{
    if ( has_attribute(selector) )
    {
	_attributes[selector] = newValue;
    }
    else
	throw eval_exception( cerror::missing_attribute, "Missing attribute " + selector);
}

void object::render( std::ostream& os, bool abbrev )
{
    if ( has_method(".render") )
    {
    	// Call the .render() method to render the object
    	objref pThis(this, [](object* o) {} );
    	literal_node* pThisLiteral = new literal_node(pThis);
    	methodcall_node* pMethodCall = new methodcall_node(".render");
    	pMethodCall->add_target(astref(pThisLiteral));
    	stringref pRendered = object::cast_or_abort<string_object>(pMethodCall->evaluate(get_context()));
    	os << pRendered->internal_value() << " ";
    	delete pMethodCall;
    }
    os << "[" << _class.name() << "]"; 
}

void object::dump( std::ostream& out)
{
    out << this << " ";
    render(out);
    out << std::endl;
}
