#ifndef CONTEXT_H
#define CONTEXT_H

#include <iostream>
#include <string>
#include <map>
#include <deque>
#include <memory>
#include "typemgr.h"
#include <parser/ast.h>

class symbol_node;

class context
{
public:


    context();
    virtual ~context();
    context(const context&)=delete;

    virtual colref new_collection();
    virtual colref new_collection(colref pCol);
    virtual void pop_collection();
    virtual void stash_state();
    virtual void restore_state();
    virtual typemgr* types(); 

    virtual objref resolve_symbol(const std::string&);
    virtual bool is_defined( const std::string&);
    virtual void assign(const std::string& name, objref value);
    virtual void reset();
    virtual colref all();
    virtual std::map<std::string,std::string> trace() const;

    friend std::ostream& operator<<( std::ostream& os, const context& c);
    virtual void dump(std::ostream& os=std::cout);
    virtual void debug() const { std::cout << (*this) << std::endl; }

private:
    typemgr* _types;
    std::deque<colref> _collections;
    std::deque<std::deque<colref>> _states;
};

class state_guard
{
public:
    state_guard(context* pContext) : _context(pContext) {}
    virtual ~state_guard() { while(newCount>0) { _context->pop_collection(); newCount--;  } }
    virtual colref new_collection() { newCount++; return _context->new_collection(); }
    virtual colref new_collection(colref pCol) { newCount++; return _context->new_collection(pCol); }


private:
    context* _context;
    int newCount{0};
};



#endif
