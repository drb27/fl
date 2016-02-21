#ifndef AST_H
#define AST_H

#include <iostream>
#include <functional>
#include <list>
#include <set>
#include <memory>
#include <interpreter/marshall.h>
#include <parser/symspec.h>
class fclass;

/// Used by ast::type() to provide run-time type information about a node
/// object. Useful for pattern matching in optimization. 
enum class asttype
{
    symbol,
	literal,
	pair,
	selector,
	sequence,
	list,
	_if,
	attr,
	methodcall,
	funcall,
	fundef,
	assign,
	_while,
	_enum,
	classdef
	};

/**
 * Represents a node in the Abstract Syntax Tree. All other nodes derive from
 * this class. The class provides common facilities and operations for all ast
 * nodes, some of which have to have implementations provided by the
 * subclasses, such as the ability to render a graph of the AST, or determining
 * which symbols are required for evaluation. 
 */
class ast
{

 public:

    virtual ~ast();

    /// Evaluates the AST in the given context hierarchy
    virtual objref evaluate(context*)=0;

    /// Returns the type of the evaluated object, if known. Implementations
    /// should return the 'object' class if the type is not known
    virtual fclass* type(context*) const;

    /// Begins a rendering of this node and subordinates in dot format
    virtual void render() const { int i=0; render_dot(i,"","",std::cout); }

    /// Returns the set of symbols required to be defined in the context to
    /// successfully evaluate this node. 
    virtual void required_symbols(std::set<symspec>& s ///< [out] The set
						       ///of symbols
				  ) const=0;

    /// Returns true if the node represents a valid lvalue (i.e. can be assigned
    /// to), or false otherwise. 
    virtual bool is_lvalue() const { return false; }

    /// Determines whether this node performs a tail call of the given function
    /// name. @note Used in optimizations
    virtual bool calls_and_returns( const std::string& fname) const;

    /// Returns the type of this node. @node used by optimizations
    virtual asttype type() const=0;

    /// Returns the *direct* subordinate ast nodes of this node. Used for
    /// traversing the AST.
    virtual void direct_subordinates( std::list<ast*>& ) const=0;

    /// Gets the type hint for this node
    /// @returns the type hint if any, or nullptr if there isn't one
    ast* get_typehint() const { return _typehint; }

    /// Sets the type hint for this node, discarding the previous value
    void set_typehint(ast* v) { _typehint=v; }

    /// Helper function used by render() to carry state information between
    /// parent nodes and subordinates during the generation of dot output. 
    virtual void render_dot(int& uuid, 
			    const std::string& parent = "",
			    const std::string& label = "",
			    std::ostream& os = std::cout) const;

 protected:

    ast();
    ast* _typehint{nullptr}; ///< Stores the type hint for this node. @note the
			     ///reason this is an ast* rather than an flcass* is
			     ///that type hints should be evaluated at the time
			     ///they are referenced - allowing for some
			     ///interesting specifications.
};


#endif
