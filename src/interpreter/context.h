#ifndef CONTEXT_H
#define CONTEXT_H

#include <string>
#include <map>
#include "typemgr.h"
#include <parser/ast.h>

class closure
{
public:
    closure();
    closure(const closure&);
    virtual ~closure();

    virtual closure& operator=(const closure& other);
    virtual void define( const std::string& name, objref obj);
    virtual void destroy( const std::string& name);

private:

    std::map<std::string,objref> _symbols;

};

class context
{

 public:
    context();
    virtual ~context();

    virtual objref resolve_symbol(const std::string&);
    virtual void assign(const std::string& name, objref value);
    typemgr& types();

 protected:

 private:
    typemgr _types;
    std::map<std::string,objref> _symbols;
};

#endif
