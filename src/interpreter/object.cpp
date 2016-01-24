#include <functional>
#include <algorithm>
#include <deque>
#include <string>
#include <vector>
#include "object.h"
#include <cassert>
#include <interpreter/eval_exception.h>
#include <parser/ast_nodes.h>
#include <logger/logger.h>

using std::string;
using std::function;
using std::deque;
using std::vector;
using std::endl;

object::object(context* pContext, fclass& c, vector<objref> params) : _class(c)
{
    if ( c.is_abstract() )
	throw eval_exception(cerror::instantiate_abstract,
			     "An attempt was made to instantiate an object of an abstract class");

    // Create the attributes for this class, and the base chain
    fclass* pCurrentClass = &c;

    auto fn = [this](fclass* pClass)
	{
	    for ( auto a : pClass->attributes() )
	    {
		_attributes[a.first] = a.second;
	    }
	};

    for ( auto pCls : c.hierarchy() )
	fn(pCls);


    construct(pContext,params);
}

object::~object()
{
    wlog_entry();
}

void object::construct(context* pContext, vector<objref>& params)
{
   // Call the constructor!
    objref pThis(this, [](object*) {});
    vector<ast*> ps(2+params.size());
    int index=2;
    ps[2]=new literal_node(objref(this));
    for ( auto p : params )
    {
	ps[index++] = new literal_node(p);
    }
    
    _class.instantiator().fn(pContext,pThis,ps);

    // Tidy up
    for ( auto p : ps )
	delete p;

}

bool object::has_attribute(const std::string& name) const
{
    return _attributes.find(name)!=_attributes.end();
}
    
objref object::get_attribute(const std::string& selector)
{
    return _attributes[selector];
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

void object::render( std::ostream& os ) const
{
    os << "[" << _class.name() << " object]"; 
}

void object::dump( std::ostream& out) const
{
    out << this << " ";
    render(out);
    out << std::endl;
}

class_object::class_object(context* pContext, fclass* pCls, fclass& cls)
    : object(pContext,cls),_value(pCls)
{
}

void class_object::render(std::ostream& os ) const
{
    os << "(class " << _value->name() << ") ";
    object::render(os);
}

int_object::int_object(context* pContext, int value, fclass& cls,bool attr) 
    : object(pContext,cls), _value(value)
{
}

bool int_object::equate( objref other ) const
{
    if (&other->get_class()!=&get_class())
	return false;

    intref other_int = std::dynamic_pointer_cast<int_object>(other);

    return (internal_value()==other_int->internal_value());
}

void int_object::render( std::ostream& os) const
{
    os << _value << " ";
    object::render(os);
}

string_object::string_object(context* pContext, const std::string& value, fclass& cls) 
    : object(pContext,cls), _value(value)
{

}

bool string_object::equate( objref other ) const
{
    if (&other->get_class()!=&get_class())
	return false;

    stringref otherRef = std::dynamic_pointer_cast<string_object>(other);
    return (internal_value()==otherRef->internal_value());
}

void string_object::render( std::ostream& os) const
{
    os << _value << " ";
    object::render(os);
}

bool_object::bool_object(context* pContext,bool b, fclass& cls)
    : _value(b),object(pContext,cls)
{
}

void bool_object::render( std::ostream& os) const
{
    os << ((_value)?"true":"false") << " ";
    object::render(os);
}

list_object::list_object(context* pContext,fclass& cls)
    : object(pContext,cls)
{
}

list_object::list_object(context* pContext,fclass& cls, std::list<objref> startList)
    : object(pContext,cls), _list(startList)
{
}

listref list_object::tail(context* pContext) const
{
    std::list<objref> startList(_list);
    startList.pop_front();
    return listref( new list_object(pContext,get_class(),startList ));
}

void list_object::render( std::ostream& os) const
{
    os << "(";
    for ( auto e : _list )
    {
	e->render(os); 
	os << " ";
    }
    os << ") ";
    object::render(os);
}

void void_object::render( std::ostream& os) const
{
    os << "(null) ";
    object::render(os);
}

fn_object::fn_object(context* pContext,
		     fclass& cls, 
		     function<marshall_fn_t> impl, 
		     deque<string> fullArgs,
		     collection&& appliedArgs)
    : _full_args(fullArgs), _expected_args(fullArgs), object(pContext,cls), _fn(impl)
{
    wlog_entry();
    _applied_arguments=std::move(appliedArgs);
}

void fn_object::render(std::ostream& os) const
{
    object::render(os);
}

void fn_object::dump( std::ostream& out) const
{
    object::dump(out);

    out << "Expected Args: ";
    for ( auto arg : _expected_args )
    {
	out << arg << " ";
    }
    out << endl;

    out << "Full Args: ";
    for ( auto arg : _full_args )
    {
	out << arg << " ";
    }
    out << endl;

    //out << "Applied Args: " << _applied_arguments << endl;

}

fnref fn_object::partial_application(context* pContext,const vector<argpair_t>& args) const
{
    wlog_entry();
    deque<string> remainingArgs(_full_args);
    collection appliedArgs;

    // For each partial application, add to the context and remove from
    // the remaining args
    for ( auto arg : args )
    {
	appliedArgs[arg.first] = arg.second;
	remainingArgs.erase(std::find(remainingArgs.begin(),remainingArgs.end(),arg.first));
    }
    
    auto result = fnref( new fn_object(pContext,get_class(),_fn,
				       remainingArgs,std::move(appliedArgs)));
    return result;

}

const deque<string>& fn_object::arglist() const
{
    return _full_args;
}

objref fn_object::operator()(context* pContext, vector<objref>& args)
{
    vector<argpair_t> argpairs;

    if ( args.size()==_full_args.size() )
    {
	// Zip 'em all up
	int index=0;
	for ( auto a : _full_args )
	{
	    argpairs.push_back(argpair_t(a,args[index++]));
	}
    }
    else
    {
	// Just the expected args
	int index=0;
	for ( auto a : args )
	{
	    argpairs.push_back(argpair_t(_expected_args[index++],a));
	}
    }

    return (*this)(pContext,argpairs);
}

objref fn_object::operator()(context* pContext, vector<argpair_t>& args)
{
    wlog_entry();
    // Is this a full or partial application?
    if (args.size()==_full_args.size() )
    {
	state_guard g(pContext);
	g.new_collection();
	vector<ast*> params;

	for ( auto arg : args )
	{
	    params.push_back( new symbol_node(arg.first) );
	    pContext->assign(arg.first,arg.second);
	}

	for ( auto aa : _applied_arguments )
	{
	    pContext->assign(aa.first,aa.second);
	}
	
	// Apply the accrued arguments to the marshall function
	auto result = _fn(pContext,params);
	return result;
    }
    else
	return partial_application(pContext,args);
}

fn_object::fn_object(context* pContext, const fn_object& other)
    : object(pContext,other.get_class()), _fn(other._fn), 
      _applied_arguments(other._applied_arguments), 
      _expected_args(other._expected_args), _full_args(other._full_args)
{
    wlog_entry();
}

