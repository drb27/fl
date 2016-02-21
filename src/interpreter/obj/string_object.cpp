#include <map>
#include <interpreter/obj/string_object.h>
#include <interpreter/obj/int_object.h>

using std::map;
using std::string;

string_object::string_object(context* pContext, const string& value, fclass& cls) 
    : object(pContext,cls), _value(value)
{

}

string_object::string_object( const string_object& other )
    : string_object(other.get_context(),other._value,other.get_class())
{

}

bool string_object::operator==( const objref other ) const
{
    if (&other->get_class()!=&get_class())
	return false;

    stringref otherRef = std::dynamic_pointer_cast<string_object>(other);
    return (internal_value()==otherRef->internal_value());
}

stringref string_object::operator[](intref index) const
{
    string substr(_value.substr(index->internal_value(),1));
    return stringref(new string_object(get_context(), substr ));
}

stringref string_object::operator+(const stringref other) const
{
    // Create a new string object as a copy of this one
    stringref pReturn = stringref( new string_object(*this) );
    pReturn->inplace_join(other);
    return pReturn;
}

void string_object::inplace_join(const stringref other)
{
    _value.append(other->_value);
}

void string_object::render( std::ostream& os, bool abbrev)
{
    os << _value << " ";
    object::render(os,abbrev);
}
