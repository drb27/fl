#include <string>
#include <list>
#include "typemgr.h"
#include "builtins.h"

using std::string;
using std::list;

typemgr::typemgr()
{
}

typemgr::~typemgr()
{

}

fclass& typemgr::lookup( const typespec& ts )
{
    // If it is in the map, return it
    auto idx = _typeMap.find(ts);
    if (idx!=_typeMap.end())
	return *((*idx).second);

    // Not in the map - check the autogenerator ...
    return check_builtin(ts);
}

fclass& typemgr::check_builtin( const typespec& ts )
{
    std::shared_ptr<fclass> pTarget;

    if (ts.full_name()=="void")
    {
	pTarget = builtins::flvoid::build_class();
    }

    if (ts.full_name()=="object")
    {
	pTarget = builtins::object::build_class();
    }

    if (ts.template_name()=="list")
    {
	pTarget = builtins::list::build_class(ts);
    }

    if (ts.template_name()=="function")
    {
	pTarget = builtins::function::build_class(ts);
    }

    if (ts.full_name()=="integer")
    {
	pTarget = builtins::integer::build_class();
    }

    if (ts.full_name()=="boolean")
    {
	pTarget = builtins::boolean::build_class();
    }

    if (pTarget!=nullptr)
    {
	_typeMap[ts] = pTarget;
	return (*pTarget);
    }
    else
	throw std::exception();
}

bool typemgr::add(fclass& cls)
{
    try
    {
	fclass& existing = lookup(cls.get_spec());
	return false;
    }
    catch (...)
    {
	_typeMap[cls.get_spec()] = std::shared_ptr<fclass>(&cls);
	return true;
    }
}
