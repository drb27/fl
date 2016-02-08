#ifndef BUILTINS_H
#define BUILTINS_H

#include <memory>
#include <interpreter/object.h>
#include <parser/ast.h>

class fclass;
class typemgr;
class fundef_node;

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
    static std::shared_ptr<fclass> build_class(typemgr*);
};

class integer
{
public:
    static std::shared_ptr<fclass> build_class(typemgr*);
};

class flfloat
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
    static std::shared_ptr<fclass> build_class(typemgr*);
};

class boolean
{
public:
    static std::shared_ptr<fclass> build_class(typemgr*);
};

class string
{
public:
    static std::shared_ptr<fclass> build_class(typemgr*);
};

class flenum
{
public:
    static std::shared_ptr<fclass> build_class(typemgr*);
};

    void build_globals(context*);
    objref obj_dump(context*, objref pThis);
    objref obj_equate(context*, objref pThis,objref pOther);
    objref obj_class(context*, objref pThis);
    objref add_integers(context*,intref a, objref b);
    objref int_dec(context*,intref pThis);
    objref int_div(context*,intref,intref);
    objref int_mod(context*,intref,intref);
    objref int_tofloat(context* pContext, intref pThis);
    objref int_to_bool(context*, intref);
    objref in_range_integers(context* pContext, intref pThis, intref min, intref max);
    objref bool_to_int(context* pContext, boolref pThis);
    objref logical_not(context* pContext,boolref a);
    objref list_size(context* pContext, listref pThis);
    objref list_head(context* pContext, listref pThis);
    objref list_append(context* pContext, listref pThis, objref e);
    objref list_join(context* pContext, listref pThis, listref pOther);
    objref list_prepend(context* pContext, listref pThis, objref e);
    objref list_tail(context* pContext, listref pThis);
    objref list_index(context* pContext, listref pThis, objref i );
    objref list_iter(context* pContext, listref pThis );
    objref list_pop(context* pContext, listref pThis );
    objref int_equate(context* pContext, intref pThis, intref pOther);
    objref int_gt(context* pContext, intref pThis, intref pOther);
    objref int_lt(context* pContext, intref pThis, intref pOther);
    objref list_dup_and_append(context* pContext, listref pThis, objref pElement);
    objref class_addmethod(context* pContext, classref pThis, fnref  fn, stringref name);
    objref string_length(context* pContext, stringref pThis);
    objref string_index(context* pContext, stringref pThis, intref index );
    objref string_add(context* pContext, stringref pThis, stringref pOther);
    objref string_join(context* pContext, stringref pThis, stringref pOther);
    objref class_methods(context* pContext, classref pThis);
    objref class_base(context* pContext, classref pThis);
    objref obj_ctor(context* pContext, objref pThis);
    objref class_derive(context* pContext, classref pThis, stringref name);
    objref class_new(context* pContext, classref pThis, listref params);
    objref class_addattr(context* pContext, classref pThis, stringref name, objref d);
    objref class_equate(context* pContext, objref pThis,objref pOther);
    objref class_attrlist(context* pContext, classref pThis );
    objref obj_assign(context* pContext, objref pThis, objref pOther);
    objref void_equate(context* pContext, objref pThis, objref pOther);
    objref rnd(context* pContext, intref a, intref b);
    objref foreach(context* pContext, objref pObj, fnref pFn );
    objref obj_is(context*,objref,objref);
    objref obj_invoke(context*,objref,stringref,listref params);
    objref list_optimise(context*,listref);
    objref list_chunks(context* pContext, listref pThis );
    objref fn_itr(context* pContext, fnref pThis );
    objref fn_name(context* pContext, fnref pThis );
    objref enum_iter(context* pContext, classref pThis );
    objref enum_str(context* pContext, enumref pThis );
    objref float_add(context* pContext, floatref a, objref b);
    objref float_to_int(context* pContext, floatref pThis);
    objref obj_convertible_to(context* pContet, objref pThis, classref pTargetClass);
    objref obj_convert(context* pContext, objref pThis, classref pTargetClass );
}

#endif
