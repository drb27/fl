#include <interpreter/collection.h>

using std::string;
using std::map;

collection::collection()
{
}

collection::~collection()
{

}

void collection::define_symbol( const symspec& s, objref o)
{
    _map[s.name()] = o;
}

objref collection::resolve_symbol(const symspec& s) const
{
    auto it = find_impl(s);
    if ( it != _map.end() )
	return (*it).second;
    else
	return objref(nullptr);
}

bool collection::is_defined(const symspec& s ) const
{
    return find_impl(s)!=_map.end();
}

map<string,objref>::const_iterator collection::find_impl( const symspec& target ) const
{
    return _map.find(target.name()); 
}

/**
 * Special implementation that transformes the symspec s into another, which has the
 * entire rqn as its bare name, and no package specification. For example, (a::b)::c
 * becomes ()::"a::b::c". This is to support proper lookup of package-scoped symbols in
 * closures, where the execution context may differ from the definition context. 
 */
void closure_collection::define_symbol( const symspec& s, objref o)
{
    symspec transformed(s.rqn(),{});
    collection::define_symbol(transformed,o);
}

/**
 * Special implementation of \e find_impl that uses the rqn of the symspec instead of the
 * bare name. This is so closures can deal with relative package specifications effectively.
 */
map<string,objref>::const_iterator 
closure_collection::find_impl( const symspec& target ) const
{
    return map().find( target.rqn() );
}
