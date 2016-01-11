#include <string.h>
#include "object.h"

using std::string;

object::object(fclass& c) : _class(c)
{
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
