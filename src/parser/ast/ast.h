#ifndef AST_H
#define AST_H

#include <iostream>
#include <functional>
#include <list>
#include <map>
#include <set>
#include <memory>
#include <interpreter/marshall.h>
#include <parser/symspec.h>

class fclass;
class selector_node;

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
	classdef,
	raise,
	range
	};

/**
 * Represents a node in the Abstract Syntax Tree. All other nodes derive from
 * this class. The class provides common facilities and operations for all ast
 * nodes, some of which have to have implementations provided by the
 * subclasses, such as the ability to render a graph of the AST, or determining
 * which symbols are required for evaluation. 
 */
class ast : public std::enable_shared_from_this<ast>
{

 public:

    virtual ~ast();

    /// A call to this method evaluates the expression represented by this node
    /// tree. Wraps a call to evaluate() with the necessary logic to detect and handle
    /// the occurence of a raised signal.
    virtual objref evaluate(context*) final;

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
    virtual void direct_subordinates( std::list<astref>& ) const=0;

    /// Attaches a selector node, describing what to do in the event that
    /// a signal is raised during evaluation of this node.
    void set_observer(selectorref pSelNode);

    /// Returns a pointer to the observer currently attached to this node. 
    /// If there is no observer, this function returns nullptr.
    selectorref observer() { return _observer; }

    /// Gets the type hint for this node
    /// @returns the type hint if any, or nullptr if there isn't one
    astref get_typehint() const { return _typehint; }

    /// Sets the type hint for this node, discarding the previous value
    void set_typehint(const astref& v) { _typehint=v; }

    /// Computes a map of ast nodes to parent nodes, for each node \em below this node in
    /// the ast.
    /// @note This node may have a parent, but it is \em not \em included in the map. It
    /// is considered to be the root node for the purposes of this calculation. 
    void compute_parent_map( std::map<astref,astref>& );

    /// Helper function used by render() to carry state information between
    /// parent nodes and subordinates during the generation of dot output. 
    virtual void render_dot(int& uuid, 
			    const std::string& parent = "",
			    const std::string& label = "",
			    std::ostream& os = std::cout) const;

    static inline unsigned long count() { return _count; }

 protected:

    ast();

    /// Evaluates the AST in the given context hierarchy
    virtual objref raw_evaluate(context*)=0;

    astref _typehint;        ///< Stores the type hint for this node. @note the
			     ///reason this is an ast* rather than an flcass* is
			     ///that type hints should be evaluated at the time
			     ///they are referenced - allowing for some
			     ///interesting specifications.

    selectorref _observer;
    sigref _signal;

 private:

    static unsigned long _count;


#ifdef AST_MONITOR_LEAKS

public:
    static void reset_leak_set() { _leakSet.clear(); }
    static const std::set<ast*>& leak_set() { return _leakSet; }

private:
    static void clear_from_leak_set( ast* n ) { _leakSet.erase(n); }
    static void monitor_for_leak( ast* n ) { _leakSet.insert(n); }

    static std::set<ast*> _leakSet;

#endif

};


#endif
