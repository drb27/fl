#include <string>
#include <map>
#include <set>
#include <sstream>
#include <list>
#include <deque>
#include <functional>
#include <algorithm>
#include "class.h"
#include <interpreter/typemgr.h>
#include <logger/logger.h>
#include <interpreter/eval_exception.h>

using std::string;
using std::list;
using std::stringstream;
using std::function;
using std::map;
using std::deque;
using std::set;

class typemgr;

typemgr* fclass::types{nullptr};

typespec::typespec( const string& nm)
    : _name(nm)
{
    wlog_entry();
}

typespec::~typespec()
{
}

const string& typespec::full_name() const
{
    return _name;
}

bool typespec::operator==( const typespec& other) const
{
    return full_name()==other.full_name();
}

int typespec::operator<( const typespec& other) const
{
    return full_name()<other.full_name();
}

int typespec::operator>( const typespec& other) const
{
    return full_name()>other.full_name();
}

fclass::fclass( const typespec& ts, fclass* pBase, bool abstract, bool builtin, 
		bool allownew, bool sealed) 
    : _ts(ts),_is_abstract(abstract),_base(pBase),_is_builtin(builtin),_allow_new(allownew),
      _is_sealed(sealed)
{
}

string fclass::name() const
{
    return _ts.full_name();
}

void fclass::add_attribute(const string& name, objref pDefault)
{
    _attributes[name] = pDefault;
}

void fclass::add_class_attribute(const string& name, objref pDefault)
{
    _class_attributes[name] = pDefault;
}

const map<string,objref>& fclass::attributes() const
{
    return _attributes;
}

const map<string,objref>& fclass::class_attributes()
{
    return _class_attributes;
}

bool fclass::has_method(const string& name )
{
    if ( _methods.find(name)!=_methods.end() )
	return true;
    else if ( !is_root() )
    {
	return base()->has_method(name);
    }
    else
	return false;
}

bool fclass::has_class_method(const string& name )
{
    if ( _class_methods.find(name)!=_class_methods.end() )
	return true;
    else if (!is_root() )
    {
	return base()->has_class_method(name);
    }
    else
	return false;
}

void fclass::add_method(const methodinfo& m)
{
    auto i = _methods.find(m.name);
    bool found = i!=_methods.end();

    if( !found || ( found && !((*i).second.sealed) ) )
    {
	_methods[m.name] = m;
    }
    else
    {
	throw eval_exception(cerror::override_sealed, "Cannot override sealed method "+m.name);
    }

}

void fclass::add_class_method(const methodinfo& m)
{
    auto i = _class_methods.find(m.name);
    bool found = i!=_class_methods.end();

    if( !found || ( found && !((*i).second.sealed) ) )
    {
	_class_methods[m.name] = m;
    }
    else
    {
	throw eval_exception(cerror::override_sealed, "Cannot override sealed method "+m.name);
    }

}

const methodinfo& fclass::lookup_class_method(const std::string& name) const
{
    auto methodIndex = _class_methods.find(name);

    if (methodIndex!=_class_methods.end())
	return (*methodIndex).second;
    else
	if ( !is_root())
	    return base()->lookup_class_method(name);
	else
	    throw eval_exception(cerror::undefined_method,"Undefined method " + name);
}

const methodinfo& fclass::lookup_method(const std::string& name) const
{
    auto methodIndex = _methods.find(name);

    if (methodIndex!=_methods.end())
	return (*methodIndex).second;
    else
	if ( !is_root())
	    return base()->lookup_method(name);
	else
	    throw eval_exception(cerror::undefined_method,"Undefined method " + name);
}

list<string> fclass::methods() const
{
    list<string> methods;
    for (auto mp : _methods )
    {
	methods.push_back(mp.first);
    }

    return methods;
}

list<string> fclass::str_attributes() const
{
    list<string> attrs;
    for (auto a : _attributes )
    {
	attrs.push_back(a.first);
    }

    return attrs;
}

deque<fclass*> fclass::hierarchy()
{
    deque<fclass*> h;
    fclass* p = this;
    h.push_front(p);

    while (!p->is_root())
    {
	p=p->base();
	h.push_front(p);
    }
    return h;
}

bool fclass::is_in_hierarchy( const fclass& other)
{
    auto h = hierarchy();
    for ( auto c : h )
    {
	if ( c == &other )
	    return true;
    }

    return false;
}

bool fclass::build_conversion_tree(fclass* pGoal,set<ctnoderef>& solutionSet)
{
    // First check the obvious
    if (pGoal==this)
	return true;

    // Is this class a decendant of pOther?
    if ( is_in_hierarchy(*pGoal) )
	return true;

    // A set of classes already in the tree
    set<fclass*> inclusionSet;

    // Root of the conversion tree
    ctnoderef root = get_all_conversions(inclusionSet,solutionSet,pGoal);

    return solutionSet.size()>0;
}

bool fclass::can_convert_to(fclass* pOther)
{
    set<ctnoderef> solutionSet;
    return build_conversion_tree(pOther,solutionSet);
}

ctnoderef fclass::get_all_conversions(set<fclass*>& inclusionSet, 
				      set<ctnoderef>& solutionSet,
				      fclass* pGoal)
{
    // Create our root, insert ourselves into the inclusion set
    ctnoderef root = ctnoderef( new conversion_tree_node(this) );
    inclusionSet.insert(this);

    // Add our children

    set<fclass*> directConversions;
    get_direct_conversions(directConversions);

    // Add direct solutions first
    for ( auto pCls : directConversions )
    {
	if (pCls==pGoal)
	{
	    ctnoderef pNode = ctnoderef(new conversion_tree_node(root,pCls));
	    solutionSet.insert(pNode);
	    inclusionSet.insert(pCls);
	}
    }

    /* foreach direct conversion */
    for ( auto pCls : directConversions )
    {
	if ( inclusionSet.find(pCls)==inclusionSet.end())
	{
	    ctnoderef childNode = pCls->get_all_conversions(inclusionSet,solutionSet,pGoal);
	    childNode->parent = root;
	    root->subordinates.insert(childNode);

	    // Have we found a solution?
	    if (pCls==pGoal)
		solutionSet.insert(childNode);
	}
    }

    // Return our root
    return root; 
}

void fclass::get_direct_conversions(std::set<fclass*>& resultSet)
{
    set<string> methodSet,filteredSet;
    all_methods(methodSet);

    for ( auto mthd : methodSet )
    {
	if (mthd.substr(0,2)=="->")
	    filteredSet.insert(mthd);
    }

    for ( auto mthd : filteredSet )
    {
	std::string trimmedName = mthd.substr(2);
	typespec ts(trimmedName);
	
	fclass* pCls = &(types->lookup(ts));
	resultSet.insert(pCls);
    }
}

void fclass::all_methods(set<string>& mset ) const
{
    // Add direct methods
    for ( auto m : methods() )
    {
	mset.insert(m);
    }

    // Call up the class hierarchy
    if (!is_root() )
	base()->all_methods(mset);

}
void fclass::all_attributes(set<string>& mset ) const
{
    // Add direct attributes
    for ( auto a : str_attributes() )
    {
	mset.insert(a);
    }

    // Call up the class hierarchy
    if (!is_root() )
	base()->all_attributes(mset);

}

const typespec& fclass::get_spec() const
{
    return _ts;
}
