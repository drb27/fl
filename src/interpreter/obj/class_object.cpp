#include <map>
#include <interpreter/obj/class_object.h>

using std::map;
using std::string;

class_object::class_object(context* pContext, fclass* pCls, fclass& cls)
    : object(pContext,cls),_value(pCls)
{
}

bool class_object::is_sealed() const
{
    return _value->is_sealed();
}

void class_object::render(std::ostream& os, bool abbrev )
{
    os << "(class " << _value->name() << ") ";
    object::render(os,abbrev);
}

bool class_object::has_attribute(const string& name) const
{
    const map<string,objref>& ca = _value->class_attributes();
    return ca.find(name)!=ca.end();
}

objref class_object::get_attribute(const string& name)
{
    return (*(_value->class_attributes().find(name))).second;
}

map<string,objref> class_object::class_attributes() 
{ 
    return _value->class_attributes();
}
