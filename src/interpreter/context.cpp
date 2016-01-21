#include <string>
#include <map>
#include <sstream>
#include "context.h"
#include "object.h"
#include "context.h"
#include "typemgr.h"
#include <interpreter/eval_exception.h>
#include <parser/ast_nodes.h>
#include <logger/logger.h>
#include <cassert>

using std::string;
using std::map;
using std::stringstream;

typemgr context::_types;

context::context()
{
}

context::context( const context& other )
{
    wlog_entry();
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
    stringstream s;
    s << other;
    map<string,string> params;
    params["other"] = s.str();
    s.clear();
    s.str("");
    s << (*this);
    params["this"] = s.str();
    wlog_entry_params(params);

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
    map<string,string> params;
    params["name"] = name;
    wlog_entry_params(params);

    if (_symbols.find(name)!=_symbols.end())
	return _symbols[name];
    else
    {
	string msg = "Unresolved symbol [" + name + "]";
	throw eval_exception(cerror::undefined_symbol,msg);
    }
}
void context::assign(const std::string& name, objref value)
{
    if (!value)
    {
	wlog(level::warning,"Assigning null value to [" + name + "]");
    }
    assert(value);
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

std::ostream& operator<<( std::ostream& os, const context& c)
{
    for ( auto symbol : c._symbols )
    {
	os << symbol.first << "(" << symbol.second << ") ";
    }

    return os;
}

bool context::is_defined( const std::string& name) const
{
    return _symbols.find(name)!=_symbols.end();
}

map<string,string> context::trace() const
{
    stringstream ss;
    map<string,string> symbols;

    for ( auto s : _symbols )
    {
	ss.clear();
	ss.str("");
	s.second->render(ss);
	symbols[s.first] = ss.str();
    }

    return symbols;
}
