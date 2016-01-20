#ifndef CONTEXT_H
#define CONTEXT_H

#include <iostream>
#include <string>
#include <map>
#include "typemgr.h"
#include <parser/ast.h>

class symbol_node;

class context
{

 public:
    context();
    context( const context&);

    virtual ~context();

    virtual objref resolve_symbol(const std::string&);
    virtual bool is_defined( const std::string&) const;
    virtual void assign(const std::string& name, objref value);
    virtual void alias(symbol_node* lvalue, symbol_node* rvalue);
    virtual void reset();
    virtual std::map<std::string,objref>& all() { return _symbols; }
    virtual std::map<std::string,std::string> trace() const;
    virtual void merge_in( const context& other);
    static typemgr& types();

    friend std::ostream& operator<<( std::ostream& os, const context& c);
    virtual void debug() const { std::cout << (*this) << std::endl; }
 protected:

 private:
    static typemgr _types;
    std::map<std::string,objref> _symbols;
};

#endif
