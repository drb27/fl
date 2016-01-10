#include <string>
#include "context.h"
#include "object.h"
#include "context.h"
#include "typemgr.h"
#include <interpreter/eval_exception.h>
using std::string;

closure::closure()
{
}

closure::closure(const closure& other)
{
}

closure::~closure()
{
}

closure& closure::operator=(const closure& other)
{
    return *this;
}

void closure::define(const string& name, objref obj)
{
    if (_symbols.find(name)!=_symbols.end())
	throw eval_exception(cerror::symbol_redefinition,"Symbol already exists");

    _symbols[name] = obj;
}

void closure::destroy(const string& name)
{
    if (_symbols.find(name)==_symbols.end())
	throw eval_exception(cerror::undefined_symbol,"Can't destroy undefined symbol");

    _symbols.erase(name);
}



context::context()
{
    typespec int_spec("integer",{});
    const fclass& int_cls = _types.lookup(int_spec);
    intref x(new int_object(42,int_cls));
    _symbols["x"] = x;
}

context::~context()
{

}

objref context::resolve_symbol(const std::string& name)
{
    if (_symbols.find(name)!=_symbols.end())
	return _symbols[name];
    else
	return objref(nullptr);
}
void context::assign(const std::string& name, objref value)
{
    _symbols[name] = value;
}

typemgr& context::types()
{
    return _types;
}
