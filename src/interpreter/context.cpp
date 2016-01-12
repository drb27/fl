#include <string>
#include "context.h"
#include "object.h"
#include "context.h"
#include "typemgr.h"
#include <interpreter/eval_exception.h>
#include <parser/ast_nodes.h>

using std::string;

typemgr context::_types;

context::context()
{
    typespec int_spec("integer",{});
    fclass& int_cls = _types.lookup(int_spec);
    intref x(new int_object(42,int_cls));
    _symbols["x"] = x;
}

context::context( const context& other )
    : _symbols(other._symbols)
{
    
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

void context::alias(symbol_node* lvalue, symbol_node* rvalue)
{
    _symbols[lvalue->name()] = _symbols[rvalue->name()];
}

typemgr& context::types()
{
    return _types;
}

void context::reset()
{
    _symbols.clear();
}
