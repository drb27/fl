#include <string>
#include "package.h"

using std::string;

package::package( const string& name, const package& base )
    : named(name), static_base(base)
{

}

package::package(const string& name ) 
    : package(name,*this)
{
}

bool package::add_class( fclass& c )
{
    if ( !classTable.contains(&c) )
    {
	classTable.set(&c);
	return true;
    }
    else
	return false;
}
