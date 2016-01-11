#ifndef AST_H
#define AST_H

#include <functional>
#include <list>
#include <memory>
#include <interpreter/marshall.h>
#include <inc/named.h>

class fclass;

class ast
{

 public:

    virtual ~ast();
    virtual objref evaluate(context*)=0;
    virtual objref evaluate(context*) const=0;
    virtual fclass* type(context*) const=0;

 protected:
    ast();

 private:

};


#endif
