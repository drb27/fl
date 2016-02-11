#ifndef TYPEMGR_H
#define TYPEMGR_H

#include <string>
#include <map>
#include "class.h"
#include <memory>

class context;

class typemgr
{

 public:
    typemgr();
    virtual ~typemgr();

    fclass& lookup( const typespec& fqn );
    bool add(fclass& cls);
    void register_builtins(context* pContext) const;

 protected:

    virtual fclass& check_builtin(const typespec&); 
    void init_builtins();

 private:
    std::map<typespec,fclass*> _typeMap;
};

#endif
