#include <interpreter/obj/void_object.h>

void_object::void_object(context* pContext, fclass& cls)
    : object(pContext,cls)
{
}

void void_object::render( std::ostream& os,bool abbrev)
{
    os << "(null) ";
    object::render(os,abbrev);
}

bool void_object::operator==( const objref other ) const
{
    return (&other->get_class()==&get_class());
}
