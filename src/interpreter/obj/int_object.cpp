#include <interpreter/obj/int_object.h>

int_object::int_object(context* pContext, int value, fclass& cls)
    : foundation_object(pContext,value,cls)
{
}

