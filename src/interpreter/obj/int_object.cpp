#include <interpreter/obj/int_object.h>

int_object::int_object(context* pContext, int value, fclass& cls)
    : object(pContext,cls), _value(value)
{
}

bool int_object::operator==( const objref other ) const
{
    if (&other->get_class()!=&get_class())
	return false;

    intref other_int = std::dynamic_pointer_cast<int_object>(other);

    return (internal_value()==other_int->internal_value());
}

void int_object::render( std::ostream& os, bool abbrev)
{
    os << _value << " ";
    object::render(os,abbrev);
}
