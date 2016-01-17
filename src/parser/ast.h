#ifndef AST_H
#define AST_H

#include <iostream>
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
    virtual fclass* type(context*) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent = "",
			    const std::string& label = "",
			    std::ostream& os = std::cout) const;

    virtual void invalidate() const;

 protected:
    ast();

 private:

};


#endif
