#ifndef CONTEXT_H
#define CONTEXT_H

#include <map>
#include "typemgr.h"
#include <parser/ast.h>

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
