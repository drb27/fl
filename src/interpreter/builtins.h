#ifndef BUILTINS_H
#define BUILTINS_H

#include <memory>
#include <interpreter/object.h>
#include <parser/ast.h>

class fclass;

namespace builtins
{

class object
{
public:
    static std::shared_ptr<fclass> build_class();
};

class integer
{
public:
    static std::shared_ptr<fclass> build_class();
};

class list
{
public:
    static std::shared_ptr<fclass> build_class(typespec);
};

class boolean
{
public:
    static std::shared_ptr<fclass> build_class();
};

class string
{

};

    objref add_integers(context*,intref a, intref b);
    objref in_range_integers(context* pContext, intref pThis, intref min, intref max);
    objref logical_not(context* pContext,boolref a);
    objref list_size(context* pContext, listref pThis);
}

#endif
