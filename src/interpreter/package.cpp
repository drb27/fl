#include <interpreter/package.h>
#include <interpreter/eval_exception.h>

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
    // Attempt to locate a matching symbol in the package hierarchy
    package* pMatchedPackage = package::resolve(this,s);

    if (pMatchedPackage)
    {
	return pMatchedPackage->context::resolve_symbol(s.name());
    }
    else
	throw eval_exception(cerror::undefined_symbol,
			     "Unresolved symbol " + s.rqn() );
}

bool package::is_defined( const symspec& s)
{
    // Attempt to locate a matching symbol in the package hierarchy
    package* pMatchedPackage = package::resolve(this,s);

    if (pMatchedPackage)
    {
	return pMatchedPackage->context::is_defined(s.name());
    }
    else
	return false;
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
    // If the speclist is empty, we are done. 
    if (speclist.size()==0)
	return currentPkg;

    // If this package is the first in the list, swallow it. 
    list<string>::const_iterator i = speclist.begin();

    if (currentPkg->name() == speclist.front() )
	i++;

    for ( ; i != speclist.end(); i++  )
    {
	if ( auto child = currentPkg->get_child(*i) )
	     currentPkg = child;
	else
	    return nullptr;
    }

    return currentPkg;
}

package* package::resolve(package* currentPackage, const symspec& s )
{
    list<string> l = s.pkg_spec();
    return resolve(currentPackage,l);
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
