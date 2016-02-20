#ifndef PACKAGE_H
#define PACKAGE_H

#include <string>
#include <map>
#include <list>

#include <interpreter/context.h>

/**
 * A class representing a package of symbols and definitions. This class derives from
 * context, which means that each package has its own, isolated context. This class
 * supports sub-packages by maintaining a list of child packages, as well as
 * implementations for various methods in the context interface to chain symbol resolution
 * operations together, so that packages 'inherit' the symbols of their parent packages.
 */
class package final : public context
{

 public:
    package(const std::string&,package*);
    virtual ~package();

    /**
     * Special implementation of resolve_symbol that attempts to find the given symbol,
     * with the associated package scope, by searching the package hierarchy. 
     */
    virtual objref resolve_symbol(const symspec&);

    /**
     * Special implementation of is_defined that attempts to find the given symbol, with
     * the associated package scope, by searching the package hierarchy. 
     */
    virtual bool is_defined( const symspec&);

    /**
     * Adds a new child package to this package. If a child package with this name already
     * exists, then the existing package is deleted, and a fresh, empty package with the
     * same name takes its place. 
     */
    virtual void add_child(const std::string&);

    /**
     * Determines if this package is a root package (i.e. has no parent package).
     */
    inline bool is_root() const { return !_parent; }

    /**
     * Returns the name of this package
     */
    inline const std::string& name() const { return _name; }

    /**
     * Returns the parent package of this package. 
     * @note if this is a root package, then \p nullptr is returned. 
     */
    inline package* parent() const { return _parent; }

    /**
     * Fetches a child package for this package. Only \e direct children of this package
     * are considered, that is to say that if \p name is a child of a child of this
     * package, then it will not be returned.
     *
     * @param name the name of the requested child package
     * @returns a pointer to the child package if it exists, otherwise \p nullptr
     */
    package* get_child(const std::string& name) const;

 protected:

    /**
     * Determines if \p name is a direct child of this package.
     *
     * @param name the name of the package
     * @returns true if it is, false otherwise
     */
    bool has_child(const std::string& name) const;

    /**
     * Attempts to resolve a package specifier, given a current package. The specifier can
     * be either absolute or relative. 
     *
     * For example, in attempting to resolve the symbol \p mypackage::sub::variable_name,
     * this method will be called with the current package as \p pkg, and the list
     * (mypackage,sub) as the \p speclist.  
     *
     * @param pkg a pointer to the package to start from
     * @param speclist a list of package names in the package specifier
     * 
     * @returns
     */
    static package* resolve(package* pkg, const std::list<std::string>& speclist);

    /**
     * Convenience method that converts symspec into an appropriate list of strings,
     * then calls the overloaded resolve() that expects a string list. 
     */
    static package* resolve(package* pkg, const symspec& s);

    /**
     * Attempts to locate an \e indirect child of this package, the route route given in
     * \p speclist. The search starts at \p pkg, and then the \p speclist route is
     * followed from there. 
     *
     * @param pkg a pointer to the package to start from
     * @param speclist a list of package names specifying the route to follow
     *
     * @returns If all packages in the chain exist, a pointer to the final package in the
     *          chain is returned. 
     */
    static package* trace_down(package* pkg, const std::list<std::string>& speclist);

 private:

    const std::string _name;	                         ///< The name of this package 
    package* const _parent;				 ///< The parent of this package
    std::map<std::string,package*> _children;		 ///< Collection of child packages
};

#endif
