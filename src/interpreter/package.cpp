#include <string>
#include "base.h"
#include "named.h"
#include "package.h"

using std::string;

package::package( const std::string& name, const package& base )
    : named(name), static_base(base)
{

}
