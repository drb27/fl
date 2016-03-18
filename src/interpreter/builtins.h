#ifndef BUILTINS_H
#define BUILTINS_H

#include <vector>
#include <map>
#include <functional>
#include <parser/ast/ast.h>
#include <interpreter/marshall.h>

class fclass;
class fundef_node;

namespace builtins
{

class object
{
public:
    static fclass* get_class();
protected:
    static fclass* build_class();
    static fclass* _class;
};

class flclass
{
public:
    static fclass* get_class();
protected:
    static fclass* build_class();
    static fclass* _class;
};

class integer
{
public:
    static fclass* get_class();
protected:
    static fclass* build_class();
    static fclass* _class;
};


class lazy
{
public:
    static fclass* get_class();
protected:
    static fclass* build_class();
    static fclass* _class;
};

class signal
{
public:
    static fclass* get_class();
protected:
    static fclass* build_class();
    static fclass* _class;
};

class eval_signal
{
public:
    static fclass* get_class();
protected:
    static fclass* build_class();
    static fclass* _class;
};

class flfloat
{ 
public:
    static fclass* get_class();
protected:
    static fclass* build_class();
    static fclass* _class;

};

class list
{
 public:
    static fclass* get_class();
protected:
    static fclass* build_class();
    static fclass* _class;
};

class function
{
public:
    static fclass* get_class();
protected:
    static fclass* build_class();
    static fclass* _class;
};

class flvoid
{
public:
    static fclass* get_class();
protected:
    static fclass* build_class();
    static fclass* _class;
};

class boolean
{
public:
    static fclass* get_class();
protected:
    static fclass* build_class();
    static fclass* _class;
};

class string
{
public:
    static fclass* get_class();
protected:
    static fclass* build_class();
    static fclass* _class;
};

class flenum
{
public:
    static fclass* get_class();
protected:
    static fclass* build_class();
    static fclass* _class;
};

    std::function<marshall_mthd_t> make_method_lambda( fnref fn );
    objref make_object(context*,fclass*,std::vector<objref>&); 
    void build_globals(context*);
    objref obj_dump(context*, objref pThis);
    objref obj_equate(context*, objref pThis,objref pOther);
    objref obj_selmatch(context*, objref pThis,objref pOther);
    objref obj_class(context*, objref pThis);
    objref obj_hash(context*, objref pThis);
    objref add_integers(context*,intref a, objref b);
    objref int_dec(context*,intref pThis);
    objref int_div(context*,intref,intref);
    objref int_mod(context*,intref,intref);
    objref int_tofloat(context* pContext, intref pThis);
    objref int_to_bool(context*, intref);
    objref int_to_string(context* pContext, intref pThis );
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
    objref int_divf(context* pContext, intref pThis, objref divisor);
    objref int_ctor(context* pContext, intref pThis, objref pOther);
    objref lazy_ctor(context*,lazyref pThis);
    objref lazy_evaluate(context*,lazyref);
    objref list_dup_and_append(context* pContext, listref pThis, objref pElement);
    objref class_constructor(context* pContext, classref pThis, fnref pCtor, lazyref pChain );
    objref class_addmethod(context* pContext, classref pThis, fnref  fn, stringref name);
    objref class_add_class_method(context* pContext, classref pThis, fnref  fn, stringref name);
    objref class_addctor(context* pContext, classref pThis, fnref  fn, listref chain);
    objref class_member(context* pContext, classref pThis, objref pObj);
    objref string_length(context* pContext, stringref pThis);
    objref string_index(context* pContext, stringref pThis, intref index );
    objref string_add(context* pContext, stringref pThis, objref pOther);
    objref string_join(context* pContext, stringref pThis, stringref pOther);
    objref class_methods(context* pContext, classref pThis);
    objref class_attributes(context* pContext, classref pThis);
    objref class_base(context* pContext, classref pThis);
    objref obj_ctor(context* pContext, objref pThis);
    objref class_new(context* pContext, classref pThis, listref params);
    objref class_addattr(context* pContext, classref pThis, stringref name, objref d);
    objref class_equate(context* pContext, objref pThis,objref pOther);
    objref class_attrlist(context* pContext, classref pThis );
    objref obj_assign(context* pContext, objref pThis, objref pOther);
    objref void_equate(context* pContext, objref pThis, objref pOther);
    objref rnd(context* pContext, intref a, intref b);
    objref print(context* pContext, objref a);
    objref I(context* pContext, objref a);
    objref foreach(context* pContext, objref pObj, fnref pFn );
    objref obj_is(context*,objref,objref);
    objref obj_invoke(context*,objref,stringref,listref params);
    objref list_optimise(context*,listref);
    objref list_chunks(context* pContext, listref pThis );
    objref list_slice(context* pContext, listref pThis, intref a, intref b );
    objref fn_name(context* pContext, fnref pThis );
    objref enum_iter(context* pContext, classref pThis );
    objref enum_str(context* pContext, enumref pThis );
    objref enum_toint(context* pContext, enumref pThis);
    objref float_add(context* pContext, floatref a, objref b);
    objref float_to_int(context* pContext, floatref pThis);
    objref obj_convertible_to(context* pContet, objref pThis, classref pTargetClass);
    objref obj_convert(context* pContext, objref pThis, classref pTargetClass );
    objref signal_ctor(context*,sigref pThis);
    objref eval_signal_ctor(context*,evalsigref pThis);
    objref obj_range(context* pContext, classref pThis, objref a, objref b );
    objref int_range(context* pContext, classref pThis, objref a, objref b );
}

#endif
