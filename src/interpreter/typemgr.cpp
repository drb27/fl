#include <string>
#include <list>
#include "typemgr.h"
#include "builtins.h"
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
    typespec ts("object",{});
    auto c = builtins::object::build_class();
    _typeMap[c->get_spec()] = c;

    c = builtins::integer::build_class(this);
    _typeMap[c->get_spec()] = c;

    c = builtins::flvoid::build_class(this);
    _typeMap[c->get_spec()] = c;

    typespec ts2=typespec("function",{});
    c = builtins::function::build_class(ts2,this);
    _typeMap[c->get_spec()] = c;

    c = builtins::boolean::build_class(this);
    _typeMap[c->get_spec()] = c;

    c = builtins::flclass::build_class(this);
    _typeMap[c->get_spec()] = c;

    c= builtins::string::build_class(this);
    _typeMap[c->get_spec()] = c;

    c= builtins::flenum::build_class(this);
    _typeMap[c->get_spec()] = c;

    typespec ts3=typespec("list",{ts});
    c = builtins::list::build_class(ts3,this);
    _typeMap[c->get_spec()] = c;

}

fclass& typemgr::check_builtin( const typespec& ts )
{
    std::shared_ptr<fclass> pTarget;

    if (ts.template_name()=="list")
    {
	pTarget = builtins::list::build_class(ts,this);
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
