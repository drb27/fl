#include <string>
#include <map>
#include <cassert>

#include <interpreter/context.h>
#include <interpreter/obj/object.h>
#include <interpreter/typemgr.h>
#include <interpreter/eval_exception.h>
#include <logger/logger.h>
#include <interpreter/collection.h>


using std::string;
using std::map;


context::context()
{
    wlog_entry();

    // Create the type manager
    _types = new typemgr();
    // Create the global collection
    new_collection();
}

context::~context()
{
    wlog_entry();
}

objref context::resolve_symbol(const symspec& s)
{
    wlog_entry();
    // Newest collections are at the head
    for ( auto col : _collections )
    {
        auto o = col->resolve_symbol(s);
	if (o)
	    return o;
    }

    // Not found in any collection
    string msg = "Unresolved symbol [" + s.rqn() + "]";
    throw eval_exception(cerror::undefined_symbol,msg);
}

colref context::new_collection()
{
    wlog_entry();
    auto pCol = colref( new collection );
    return new_collection(pCol);
}

colref context::new_collection( colref pCol)
{
    wlog_entry();
    _collections.push_front(pCol);
    return pCol;
}

colref context::pop_collection()
{
    wlog_entry();

    if ( _collections.size()>1 )
    {
	auto r = _collections.front();
	_collections.pop_front();
	return r;
    }
    else
	throw stack_underflow_exception();
}

colref context::current_collection()
{
    return _collections.front();
}

typemgr* context::types()
{
    wlog_entry();
    return _types;
}

bool context::is_defined(const symspec& s)
{
    wlog_entry();

    for ( auto col : _collections )
    {
	auto o = col->resolve_symbol(s);
	if (o) 
	    return true;
    }

    return false;
}

void context::assign(const symspec& s, objref value )
{
    wlog_entry();
    (*_collections.front()).define_symbol(s,value);
}

void context::reset()
{
    wlog_entry();
    _collections.clear();
    new_collection();
}

void context::dump( std::ostream& os )
{
    wlog_entry();
    int level_index=0;

    // Iterate over the levels (from global->most local)
    for ( auto level_iter = _collections.rbegin();
	  level_iter != _collections.rend();
	  level_iter++ )
    {
	colref pCol = *level_iter;

	os << " LEVEL " << level_index << "----->" << std::endl;

	// Iterate over symbols in this collection
	for ( const auto s : *pCol )
	{
	    os << s.first << ": ";
	    s.second->render(os);
	    os << std::endl;
	}
    }
}

std::ostream& operator<<( std::ostream& os, const context& c)
{
    wlog_entry();
    int level_index=0;

    // Iterate over the levels (from global->most local)
    for ( auto level_iter = c._collections.rbegin();
	  level_iter != c._collections.rend();
	  level_iter++ )
    {
	colref pCol = *level_iter;

	os << " LEVEL " << level_index << "----->" << std::endl;

	// Iterate over symbols in this collection
	for ( auto symbol : *pCol )
	{
	    os << symbol.first << "(" << symbol.second << ") ";
	}
    }

    return os;
}

map<string,string> context::trace() const
{
    wlog_entry();
    map<string,string> m;
    return m;
}

bool context::unwind(colref c)
{
    // Unwind the stack until the current frame is equal to c
    try
    {
	while ( c!=current_collection() )
	{
	    pop_collection();
	}
    }
    catch ( stack_underflow_exception& e )
    {
	// Whoops, we were about to pop the global context!!
	return false;
    }   

    return true;
}
