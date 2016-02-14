#include <interpreter/obj/int_object.h>
#include <interpreter/obj/string_object.h>
#include <interpreter/obj/enum_object.h>

enum_object::enum_object(context* pContext, int value, 
			 const std::string& name, fclass& cls) 
    : int_object(pContext,value,cls),_name(name)
{
    
}

void enum_object::render( std::ostream& os, bool abbrev)
{
    os << _name << " ";
    object::render(os,abbrev);
}

stringref enum_object::str()
{
    return stringref( new string_object(get_context(), _name ));

}
