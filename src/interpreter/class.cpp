#include <string>
#include "named.h"
#include "base.h"
#include "class.h"

using std::string;

fclass::fclass( const string& name, const fclass& base) 
    : named(name),static_base<fclass>(base)
{

}

fclass::fclass(const string& name)
    : named(name),static_base<fclass>(*this)
{
}
