#ifndef CONTEXT_H
#define CONTEXT_H

#include <iostream>
#include <exception>
#include <string>
#include <map>
#include <deque>
#include <memory>
#include <parser/symspec.h>
#include "typemgr.h"
#include <parser/ast/ast.h>

class symbol_node;
class state_guard;

class stack_underflow_exception : public std::exception
{
public:
    const char* what() const throw() { return "Context stack underflow"; }
};

class context
{
public:


    context();
    virtual ~context();
    context(const context&)=delete;

    virtual colref new_collection();
    virtual colref new_collection(colref pCol);
    virtual colref pop_collection();
    virtual typemgr* types(); 

    virtual colref current_collection();
    virtual objref resolve_symbol(const symspec&);
    virtual bool is_defined( const symspec&);
    virtual void assign(const symspec& name, objref value);
    virtual void reset();
    virtual std::map<std::string,std::string> trace() const;

    friend std::ostream& operator<<( std::ostream& os, const context& c);
    virtual void dump(std::ostream& os=std::cout);
    virtual void debug() const { std::cout << (*this) << std::endl; }

    virtual const astref& root_node() { return _root_node; }
    virtual void set_root_node(const astref& n) { _root_node=n; }

    virtual bool unwind(colref);

private:
    typemgr* _types;
    std::deque<colref> _collections;
    astref _root_node{nullptr};
};

class state_guard
{
public:
    state_guard(context* pContext) : _context(pContext)
    {
	_orig_colref = pContext->current_collection();
    }

    virtual ~state_guard() 
    {
	_context->unwind(_orig_colref);
    }

private:
    context* _context;
    colref _orig_colref;
};



#endif
