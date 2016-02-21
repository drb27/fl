#include <sstream>
#include <parser/symspec.h>

using std::string;
using std::stringstream;

string symspec::rqn() const
{
    stringstream output;
    for ( auto s : _pkg_spec )
    {
	output << s << "::";
    }
    output << _name;
    return output.str();
} 

bool symspec::operator<(const symspec& other) const
{
    return rqn()<other.rqn();
}
