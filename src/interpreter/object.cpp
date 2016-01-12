#include <functional>
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

void fn_object::apply_argument( objref arg )
{
    // Pop the next expected argument name
    string argname = _expected_args.front();
    _expected_args.pop_front();

    // Add the symbol to the applied arguments context
    _applied_arguments.assign(argname,arg);
}

bool fn_object::has_all_arguments() const
{
    return _expected_args.size()==0;
}

objref fn_object::operator()(void)
{
    // Prepare a vector<ast*> of symbol_nodes, one for each expected argument
    vector<ast*> params;
    for ( auto p : _full_args )
    {
	params.push_back( new symbol_node(p) );
    }
    
    // Apply the accrued arguments to the marshall function, and return the result!
    return _fn(&_applied_arguments,params);
}
