#include <interpreter/obj/bool_object.h>

bool_object::bool_object(context* pContext,bool b, fclass& cls)
    : _value(b),object(pContext,cls)
{
}

bool bool_object::operator==( const objref other ) const
{
    if (&other->get_class()!=&get_class())
	return false;

    boolref other_bool = std::dynamic_pointer_cast<bool_object>(other);

    return (internal_value()==other_bool->internal_value());
}

void bool_object::render( std::ostream& os, bool abbrev)
{
    os << ((_value)?"true":"false") << " ";
    object::render(os,abbrev);
}
