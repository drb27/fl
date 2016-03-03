#include <interpreter/obj/lazy_object.h>

lazy_object::lazy_object(context* pContext, const astref& value, fclass& cls)
    : object(pContext,cls), _value(value)
{
}

const astref& lazy_object::internal_value() const
{
    return _value;
}

