#include <string.h>
#include "object.h"

using std::string;

const fclass object::_objectclass("object"); 
const fclass ftinteger::_intclass("integer",object::root_object_class());
std::map<const fclass*,const fclass*> ftlist::_typemap;

object::object(const fclass& c) : _class(c)
{
}

object::~object()
{

}

bool object::has_attribute(const std::string& name) const
{
    return _attributes.find(name)!=_attributes.end();
}

const fclass& object::root_object_class()
{
    return _objectclass;
}

void object::render( std::ostream& os ) const
{
    os << "[" << _class.name() << " object]"; 
}

void ftinteger::render( std::ostream& os ) const
{
    os << _value <<  " [" << get_class().name() << " object]"; 
}

ftinteger::ftinteger(int v)
    : _value(v), object( _intclass )
{

}

const fclass& ftlist::class_factory( const fclass& etype )
{
    if ( _typemap.find(&etype)!=_typemap.end() )
    {
	// List class for this type already exists
	return *(_typemap[&etype]);
    }
    else
    {
	// Need to create and insert the list class
	fclass* fc = new fclass("list of " + etype.name(), root_object_class() );
	_typemap[&etype] = fc;
	return *fc;
    }

}

ftlist::ftlist(const fclass& etype)
    : _etype(etype), object( class_factory(etype) )
{
    
}

void ftlist::append( object* obj )
{
    _objects.push_back(obj);
}

void ftlist::render(std::ostream& os) const
{
    os << "(...) [list of " << _etype.name() << "]";
}
