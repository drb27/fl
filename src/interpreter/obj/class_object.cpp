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

/**
 *  Called when a class object is used as the guard in a selector statement (and also in
 *  the handle clause of an observe statement, which is implemented internally as a
 *  selector). The philosophy is that the guard makes the decision as to what matches. In
 *  the case of a class object, this implementation matches itself, but also any class
 *  downstream in the class hierarchy. 
 *
 */
bool class_object::selector_match( const objref other) const
{
    classref pClsOther = std::dynamic_pointer_cast<class_object>(other);
    if (!pClsOther)
	return (*this)==other;
    else
	return pClsOther->_value->is_a( *_value );
}
