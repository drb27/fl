#include <string.h>
#include "context.h"
#include "object.h"
#include "context.h"
#include "typemgr.h"

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
