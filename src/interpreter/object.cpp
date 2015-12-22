#include <string.h>
#include "object.h"

using std::string;

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

void object::render( std::ostream& os ) const
{
    os << "[" << _class.name() << " object]"; 
}
