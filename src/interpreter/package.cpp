#include <interpreter/package.h>

using std::string;
using std::map;
using std::list;

package::package(const string& name, package* parent)
    : context(),_name(name),_parent(parent)
{
}

package::~package()
{
    // Cascade delete to children
    for( auto c : _children)
    {
	delete c.second;
    }

    _children.clear();
}

objref package::resolve_symbol(const symspec& s)
{
    // TODO
    return context::resolve_symbol(s);
}

bool package::is_defined( const symspec& s)
{
    // TODO
    return context::is_defined(s);
}

void package::add_child(const string& name)
{
    if ( _children.find(name)!=_children.end() )
	delete _children[name];

    auto p = new package(name,this);
    _children[name] = p;
}

bool package::has_child(const string& name ) const
{
    return _children.find(name)!=_children.end();
}

package* package::get_child(const string& name ) const
{
    if (_children.find(name)!=_children.end() )
	return _children.at(name);
    else
	return nullptr;
}

package* package::trace_down(package* currentPkg, const list<string>& speclist )
{
    for ( auto name : speclist )
    {
	if ( auto child = currentPkg->get_child(name) )
	     currentPkg = child;
	else
	    return nullptr;
    }

    return currentPkg;
}

package* package::resolve(package* currentPackage, const list<string>& speclist )
{ 

    // Can we get to the result from this node?
    package* pTargetPkg;
    if ( pTargetPkg = trace_down(currentPackage,speclist) )
    {
	// Yes - return the target package
	return pTargetPkg;
    }
    else
    {
	// No - try parents, until we get to the root
	if ( currentPackage->is_root() )
	    return nullptr;
	else
	    return resolve(currentPackage->_parent, speclist );
    }
    
    // Still no match? Then package spec is invalid from here
    return nullptr;
}
