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

objref context::resolve_symbol(const string& name)
{
    wlog_entry();
    // Newest collections are at the head
    for ( auto col : _collections )
    {
        auto i = col->find(name);
	if (i!=col->end())
	    return (*i).second;
    }

    // Not found in any collection
    string msg = "Unresolved symbol [" + name + "]";
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
    auto r = _collections.front();
    _collections.pop_front();
    return r;
}

colref context::current_collection()
{
    return _collections.front();
}

void context::stash_state()
{
    wlog_entry();
    _states.push_back(_collections);
}

void context::restore_state()
{
    wlog_entry();
    _collections = std::move( _states.back() );
    _states.pop_back();
}

typemgr* context::types()
{
    wlog_entry();
    return _types;
}

bool context::is_defined(const string& name)
{
    wlog_entry();

    for ( auto col : _collections )
    {
	auto i = col->find(name);
	if ( i!=col->end())
	    return true;
    }

    return false;
}

void context::assign(const string& name, objref value )
{
    wlog_entry();
    (*_collections.front())[name] = value;
}

void context::reset()
{
    wlog_entry();
    _collections.clear();
    _states.clear();
    new_collection();
}

colref context::all()
{
    wlog_entry();
    auto pooled = colref( new collection );

    for ( auto i = _collections.rbegin(); i!= _collections.rend(); i++ )
    {
	for ( auto p : *(*i) )
	{
	    (*pooled)[p.first] = p.second;
	}
    }

    return pooled;
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
	for ( auto s : *pCol )
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
