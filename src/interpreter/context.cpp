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
{
    typespec ts("function",{});
    auto& fnclass = types().lookup(ts);

    for ( auto s : other._symbols)
    {
	if ( &(s.second->get_class()) == &fnclass )
	{
	    fnref cloned_fn( new fn_object(*std::dynamic_pointer_cast<fn_object>(s.second)));
	    assign(s.first,cloned_fn);
	}
	else
	    assign(s.first,s.second);
    }
}

void context::merge_in( const context& other)
{

    for ( auto s : other._symbols )
    {
	assign(s.first,s.second);
    }
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
