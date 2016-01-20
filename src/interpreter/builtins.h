#ifndef BUILTINS_H
#define BUILTINS_H

#include <memory>
#include <interpreter/object.h>
#include <parser/ast.h>

class fclass;
class typemgr;

namespace builtins
{

class object
{
public:
    static std::shared_ptr<fclass> build_class();
};

class flclass
{
public:
    static std::shared_ptr<fclass> build_class();
};

class integer
{
public:
    static std::shared_ptr<fclass> build_class(typemgr*);
};

class list
{
public:
    static std::shared_ptr<fclass> build_class(typespec,typemgr*);
};

class function
{
public:
    static std::shared_ptr<fclass> build_class(typespec,typemgr*);
};

class flvoid
{
public:
    static std::shared_ptr<fclass> build_class();
};

class boolean
{
public:
    static std::shared_ptr<fclass> build_class(typemgr*);
};

class string
{

};
    
    objref obj_dump(context*, objref pThis);
    objref obj_class(context*, objref pThis);
    objref add_integers(context*,intref a, intref b);
    objref int_dec(context*,intref pThis);
    objref in_range_integers(context* pContext, intref pThis, intref min, intref max);
    objref logical_not(context* pContext,boolref a);
    objref list_size(context* pContext, listref pThis);
    objref list_head(context* pContext, listref pThis);
    objref list_append(context* pContext, listref pThis, objref e);
    objref list_tail(context* pContext, listref pThis);
    objref int_equate(context* pContext, intref pThis, intref pOther);
    objref int_gt(context* pContext, intref pThis, intref pOther);
    objref list_dup_and_append(context* pContext, listref pThis, objref pElement);

}

#endif
