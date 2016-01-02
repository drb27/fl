#include <string>
#include <list>
#include "typemgr.h"

using std::string;
using std::list;

typemgr::typemgr()
{
}

typemgr::~typemgr()
{

}

const fclass& typemgr::lookup( const typespec& ts )
{
    // If it is in the map, return it
    auto idx = _typeMap.find(ts);
    if (idx!=_typeMap.end())
	return *((*idx).second);

    // Not in the map - check the autogenerator ...
    return check_builtin(ts);
}

const fclass& typemgr::check_builtin( const typespec& ts )
{
    fclass* pTarget=nullptr;

    if (ts.full_name()=="object")
    {
	list<typespec> l;
	typespec ts("object",l);
	pTarget = new fclass(ts);
    }

    if (ts.template_name()=="list")
    {
	pTarget = new fclass(ts);
    }

    if (ts.full_name()=="int")
    {
	typespec ts("int",{});
	pTarget = new fclass(ts);
    }

    if (pTarget!=nullptr)
    {
	_typeMap[ts] = pTarget;
	return (*pTarget);
    }
    else
	throw std::exception();
}

bool typemgr::add(const fclass& cls)
{
    try
    {
	const fclass& existing = lookup(cls.get_spec());
	return false;
    }
    catch (...)
    {
	_typeMap[cls.get_spec()] = &cls;
	return true;
    }
}
