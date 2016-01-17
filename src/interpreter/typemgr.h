#ifndef TYPEMGR_H
#define TYPEMGR_H

#include <string>
#include <map>
#include "class.h"
#include <memory>

class typemgr
{

 public:
    typemgr();
    virtual ~typemgr();

    fclass& lookup( const typespec& fqn );
    bool add(fclass& cls);

 protected:

    virtual fclass& check_builtin(const typespec&); 
    void init_builtins();

 private:
    std::map<typespec,std::shared_ptr<fclass>> _typeMap;
};

#endif
