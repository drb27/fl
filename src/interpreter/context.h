#ifndef CONTEXT_H
#define CONTEXT_H

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
    virtual void assign(const std::string& name, objref value);
    virtual void alias(symbol_node* lvalue, symbol_node* rvalue);
    virtual void reset();
    virtual void merge_in( const context& other);
    static typemgr& types();

 protected:

 private:
    static typemgr _types;
    std::map<std::string,objref> _symbols;
};

#endif
