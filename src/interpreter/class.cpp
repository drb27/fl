#include <string>
#include <map>
#include <sstream>
#include <list>
#include <functional>
#include "named.h"
#include "class.h"
#include <logger/logger.h>
#include <interpreter/eval_exception.h>

using std::string;
using std::list;
using std::stringstream;
using std::function;
using std::map;

typespec::typespec( const string& nm, const list<typespec>& prms )
    : _name(nm)
{
    for (auto p : prms )
    {
	_params.push_back(p);
    }
    validate();
    _full_name=calc_full_name();
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

string typespec::calc_full_name()
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

const string& typespec::full_name() const
{
    return _full_name;
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

fclass::fclass( const typespec& ts, fclass* pBase, bool abstract) 
    : _ts(ts),_is_abstract(abstract),_base(pBase)
{
}

string fclass::name() const
{
    return _ts.full_name();
}

void fclass::add_attribute(const string& name, fclass* ftype, objref pDefault)
{
    _attributes[name] = ftype;
}

void fclass::add_method(const string& name, function<marshall_mthd_t> delegate)
{
    _methods[name] = delegate;
}

function<marshall_mthd_t> fclass::lookup_method(const std::string& name) const
{
    auto methodIndex = _methods.find(name);

    if (methodIndex!=_methods.end())
	return (*methodIndex).second;
    else
	if ( !is_root())
	    return base()->lookup_method(name);
	else
	    throw eval_exception(cerror::undefined_method,"Undefined method " + name);
}

list<string> fclass::methods() const
{
    list<string> methods;
    for (auto mp : _methods )
    {
	methods.push_back(mp.first);
    }

    return methods;
}
