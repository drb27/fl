#include <string.h>
#include "typemgr.h"

using std::string;

typemgr::typemgr()
{
}

typemgr::~typemgr()
{

}

const fclass& typemgr::lookup( const typespec& ts )
{
    // If it is in the map, return it
    auto idx = _typeMap.find(ts.full_name());
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
	pTarget = new fclass("object");
    }

    if (pTarget!=nullptr)
    {
	_typeMap[ts.full_name()] = pTarget;
	return (*pTarget);
    }
    else
	throw std::exception();
}
