#include <functional>
#include <algorithm>
#include <deque>
#include <string>
#include <vector>
#include "object.h"
#include <interpreter/eval_exception.h>
#include <parser/ast_nodes.h>
#include <logger/logger.h>

using std::string;
using std::function;
using std::deque;
using std::vector;
using std::endl;

object::object(fclass& c) : _class(c)
{
    if ( c.is_abstract() )
	throw eval_exception(cerror::instantiate_abstract,"An attempt was made to instantiate an object of an abstract class");
}

object::~object()
{

}

bool object::has_attribute(const std::string& name) const
{
    return _attributes.find(name)!=_attributes.end();
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

int_object::int_object(int value, fclass& cls) : object(cls), _value(value)
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

bool_object::bool_object(bool b, fclass& cls)
    : _value(b),object(cls)
{
}

void bool_object::render( std::ostream& os) const
{
    os << ((_value)?"true":"false") << " ";
    object::render(os);
}

list_object::list_object(fclass& cls)
    : object(cls)
{
}

list_object::list_object(fclass& cls, std::list<objref> startList)
    : object(cls), _list(startList)
{
}

listref list_object::tail() const
{
    std::list<objref> startList(_list);
    startList.pop_front();
    return listref( new list_object(get_class(),startList ));
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

void null_object::render( std::ostream& os) const
{
    os << "null ";
    object::render(os);
}

fn_object::fn_object(fclass& cls, function<marshall_fn_t> impl, deque<string> args)
    : _expected_args(args), _full_args(args), object(cls), _fn(impl)
{
    wlog_entry();
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

    out << "Applied Args: " << _applied_arguments << endl;

}

fnref fn_object::partial_application(const vector<argpair_t>& args) const
{
    wlog_entry();
    deque<string> remainingArgs(_full_args);
    context newContext;

    // For each partial application, add to the context and remove from
    // the remaining args
    for ( auto arg : args )
    {
	newContext.assign(arg.first,arg.second);
	remainingArgs.erase(std::find(remainingArgs.begin(),remainingArgs.end(),arg.first));
    }
    
    auto result = fnref( new fn_object(get_class(),_fn,remainingArgs));
    result->_applied_arguments.merge_in(newContext);
    return result;

}

void fn_object::apply_argument( objref arg )
{
    wlog_entry();
    // Pop the next expected argument name
    string argname = _expected_args.front();
    _expected_args.pop_front();

    // Add the symbol to the applied arguments context
    _applied_arguments.assign(argname,arg);
}

void fn_object::apply_argument( const string& name, objref arg )
{
    wlog_entry();
    // Check the named argument is valid
    deque<string>::iterator i = std::find(_expected_args.begin(),_expected_args.end(),name);
    
    if (i==_expected_args.end())
    {
	throw std::exception();
    }

    // Remove from expected arguments
    _expected_args.erase(i);

    // Add th1e symbol to the applied arguments context
    _applied_arguments.assign(name,arg);
}

const deque<string>& fn_object::arglist() const
{
    return _full_args;
}

objref fn_object::operator()(context* pContext, vector<argpair_t>& args)
{
    wlog_entry();
    // Is this a full or partial application?
    if (args.size()==_full_args.size() )
    {
	context tempContext(*pContext);
	context savedAppliedArgs(_applied_arguments);

	// Apply each of the arguments
	for ( auto p : args )
	{
	    apply_argument(p.first,p.second);
	}

	// Prepare a vector<ast*> of symbol_nodes, one for each expected argument
	vector<ast*> params;
	for ( auto p : _full_args )
	{
	    params.push_back( new symbol_node(p) );
	}
    
	wlog(level::debug,"Merging in applied arguments...");
	tempContext.merge_in(_applied_arguments);

	wlog_trace("Post-merge context: ",tempContext.trace());

	// Apply the accrued arguments to the marshall function
	auto result = _fn(&tempContext,params);

	// Reset arguments for another invocation
	_expected_args = _full_args;
	_applied_arguments = savedAppliedArgs;

	return result;
    }
    else
	return partial_application(args);
}

fn_object::fn_object(const fn_object& other)
    : object(other.get_class()), _fn(other._fn), _applied_arguments(other._applied_arguments), 
	     _expected_args(other._expected_args), _full_args(other._full_args)
{
    wlog_entry();
}

