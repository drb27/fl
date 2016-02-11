#include <string>
#include <list>
#include "typemgr.h"
#include "builtins.h"
#include "context.h"
#include <logger/logger.h>

using std::string;
using std::list;

typemgr::typemgr()
{
    wlog(level::debug,"Setting type manager in fclass");
    fclass::types = this;
    init_builtins();
}

typemgr::~typemgr()
{
    wlog_entry();
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

void typemgr::init_builtins()
{
    wlog_entry();

    auto c = builtins::object::get_class();
    _typeMap[c->get_spec()] = c;

    c = builtins::integer::get_class();
    _typeMap[c->get_spec()] = c;

    c = builtins::flfloat::get_class();
    _typeMap[c->get_spec()] = c;

    c = builtins::flvoid::get_class();
    _typeMap[c->get_spec()] = c;

    c = builtins::function::get_class();
    _typeMap[c->get_spec()] = c;

    c = builtins::boolean::get_class();
    _typeMap[c->get_spec()] = c;

    c = builtins::flclass::get_class();
    _typeMap[c->get_spec()] = c;

    c= builtins::string::get_class();
    _typeMap[c->get_spec()] = c;

    c= builtins::flenum::get_class();
    _typeMap[c->get_spec()] = c;

    c = builtins::list::get_class();
    _typeMap[c->get_spec()] = c;

}

void typemgr::register_builtins(context* pContext) const
{
}

fclass& typemgr::check_builtin( const typespec& ts )
{
    fclass* pTarget;

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
	_typeMap[cls.get_spec()] = &cls;
	return true;
    }
}
