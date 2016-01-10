#include <string>
#include <sstream>
#include <list>
#include <functional>
#include "named.h"
#include "base.h"
#include "class.h"

using std::string;
using std::list;
using std::stringstream;
using std::function;

typespec::typespec( const string& nm, const list<typespec>& prms )
    : _name(nm)
{
    for (auto p : prms )
    {
	_params.push_back(p);
    }
    validate();
}

typespec::~typespec()
{
}

int typespec::param_count() const
{
    return _params.size();
}

const typespec& fclass::get_spec() const
{
    return _ts;
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

bool typespec::operator==( const typespec& other) const
{
    return full_name()==other.full_name();
}

int typespec::operator<( const typespec& other) const
{
    return full_name()<other.full_name();
}

int typespec::operator>( const typespec& other) const
{
    return full_name()>other.full_name();
}

void typespec::validate() const
{
    if ( (template_name()=="list") && param_count()!=1 )
	throw std::logic_error("Invalid typespec");

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

void fclass::add_attribute(const string& name, fclass* ftype, object* pDefault)
{
    _attributes[name] = ftype;
}

void fclass::add_method(const string& name, function<marshall_fn_t> delegate)
{
    _methods[name] = delegate;
}

function<marshall_fn_t> fclass::lookup_method(const std::string& name)
{
    return _methods[name];
}

