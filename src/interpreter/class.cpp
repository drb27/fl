#include <string>
#include <sstream>
#include <list>
#include "named.h"
#include "base.h"
#include "class.h"

using std::string;
using std::list;
using std::stringstream;

typespec::typespec( const string& nm )
    : _name(nm)
{
}

typespec::~typespec()
{
}

int typespec::param_count() const
{
    return _params.size();
}

const string& typespec::template_name() const
{
    return _name;
}

string typespec::full_name() const
{
    stringstream s;
    s << template_name();

    if (param_count())
    {
	s << "<";
	for ( auto v : _params )
	{
	    s << v.full_name() << ",";
	}
    }
    
    string result = s.str();
    if (param_count())
	return result.erase(result.length()-1) + ">";
    else
	return result;
}

const list<typespec>& typespec::params() const
{
    return _params;
}

list<typespec>& typespec::params()
{
    return _params;
}

void typespec::push_param( typespec& t )
{
    _params.push_back(t);
}

bool typespec::operator==( const typespec& other) const
{
    return full_name()==other.full_name();
}

fclass::fclass( const typespec& ts, const fclass& base) 
    : _ts(ts),static_base<fclass>(base)
{

}

fclass::fclass(const typespec& ts)
    : _ts(ts),static_base<fclass>(*this)
{
}

string fclass::name() const
{
    return _ts.full_name();
}
