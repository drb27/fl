#ifndef AST_H
#define AST_H

#include <iostream>
#include <functional>
#include <list>
#include <set>
#include <memory>
#include <interpreter/marshall.h>
#include <inc/named.h>

class fclass;

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
	_enum
	};

class ast
{

 public:

    virtual ~ast();
    virtual objref evaluate(context*)=0;
    virtual fclass* type(context*) const;
    virtual void render() const { int i=0; render_dot(i,"","",std::cout); }
    virtual void render_dot(int& uuid, 
			    const std::string& parent = "",
			    const std::string& label = "",
			    std::ostream& os = std::cout) const;

    virtual void invalidate() const;
    virtual void required_symbols(std::set<std::string>&) const=0;
    virtual bool is_lvalue() const { return false; }

    virtual bool calls_and_returns( const std::string& fname) const;
    virtual asttype type() const=0;
    virtual void direct_subordinates( std::list<ast*>& ) const=0;

    ast* get_typehint() const { return _typehint; }
    void set_typehint(ast* v) { _typehint=v; }

 protected:
    ast();
    ast* _typehint;
 private:

};


#endif
