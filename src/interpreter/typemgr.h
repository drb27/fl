#ifndef TYPEMGR_H
#define TYPEMGR_H

#include <string>
#include <map>
#include "class.h"

class typemgr
{

 public:
    typemgr();
    virtual ~typemgr();

    const fclass& lookup( const typespec& fqn );

 protected:

    virtual const fclass& check_builtin(const typespec&); 

 private:
    std::map<std::string,const fclass*> _typeMap;
};

#endif
