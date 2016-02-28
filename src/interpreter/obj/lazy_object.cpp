#include <interpreter/obj/lazy_object.h>

lazy_object::lazy_object(context* pContext, ast* value, fclass& cls)
    : foundation_object(pContext,value,cls)
{
}

