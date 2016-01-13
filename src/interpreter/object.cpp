#include <functional>
#include <algorithm>
#include <deque>
#include <string>
#include <vector>
#include "object.h"
#include <interpreter/eval_exception.h>
#include <parser/ast_nodes.h>

using std::string;
using std::function;
using std::deque;
using std::vector;

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

int_object::int_object(int value, fclass& cls) : object(cls), _value(value)
{
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
}

void fn_object::render(std::ostream& os) const
{
    object::render(os);
}

fnref fn_object::partial_application(const vector<argpair_t>& args) const
{
    // Creates a NEW function object with args.size() fewer args
    return fnref(nullptr);
}

void fn_object::apply_argument( objref arg )
{
    // Pop the next expected argument name
    string argname = _expected_args.front();
    _expected_args.pop_front();

    // Add the symbol to the applied arguments context
    _applied_arguments.assign(argname,arg);
}

void fn_object::apply_argument( const string& name, objref arg )
{
    // Check the named argument is valid
    auto i = std::find(_expected_args.begin(),_expected_args.end(),name);

    if (i==_expected_args.end());
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

objref fn_object::operator()(vector<argpair_t>& args)
{
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
    
    // Apply the accrued arguments to the marshall function
    auto result = _fn(&_applied_arguments,params);

    // Reset arguments for another invocation
    _expected_args = _full_args;
    _applied_arguments.reset();
}
