#include <interpreter/obj/float_object.h>
#include <interpreter/eval_exception.h>

float_object::float_object(context* pContext,double value,fclass& cls)
    : object(pContext,cls), _value(value)
{
}

bool float_object::operator==( const objref other ) const
{
    throw eval_exception(cerror::equate_float, "Can't equate to floats");
}

void float_object::render( std::ostream& os, bool abbrev)
{
    os << _value << " ";
    object::render(os);
}
