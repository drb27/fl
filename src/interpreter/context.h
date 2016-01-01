#ifndef CONTEXT_H
#define CONTEXT_H

#include "typemgr.h"

class context
{

 public:
    context();
    virtual ~context();

    typemgr& types();

 protected:

 private:
    typemgr m_types;
    
};

#endif
