#include <string.h>
#include "builtins.h"
#include <interpreter/class.h>
#include <interpreter/context.h>
#include <parser/callable.h>

#define N_INT(x) (x->internal_value())

namespace builtins
{
    std::shared_ptr<fclass> object::build_class()
    {
	typespec spec("object",{});
	std::shared_ptr<fclass> pCls(new fclass(spec));
	return pCls;
    }

    std::shared_ptr<fclass> integer::build_class()
    {
	typespec spec("integer",{});
	std::shared_ptr<fclass> pCls(new fclass(spec));
	pCls->add_method("add", make_marshall(&builtins::add_integers));
	pCls->add_method("in_range", make_marshall(&builtins::in_range_integers));
	pCls->add_method("eq", make_marshall(&builtins::int_equate));
	return pCls;
    }

    std::shared_ptr<fclass> flvoid::build_class()
    {
	typespec spec("void",{});
	std::shared_ptr<fclass> pCls(new fclass(spec,true));
	return pCls;
    }

    std::shared_ptr<fclass> list::build_class(typespec spec)
    {
	std::shared_ptr<fclass> pCls(new fclass(spec));
	pCls->add_method("size", make_marshall(&builtins::list_size));
	pCls->add_method("head", make_marshall(&builtins::list_head));
	pCls->add_method("append", make_marshall(&builtins::list_append));
	pCls->add_method("tail", make_marshall(&builtins::list_tail));
	return pCls;
    }

    std::shared_ptr<fclass> function::build_class(typespec spec)
    {
	std::shared_ptr<fclass> pCls(new fclass(spec));
	return pCls;
    }

    std::shared_ptr<fclass> boolean::build_class()
    {
	typespec spec("boolean",{});
	std::shared_ptr<fclass> pCls(new fclass(spec));
	pCls->add_method("not", make_marshall(&builtins::logical_not));
	return pCls;
    }

    objref add_integers(context* pContext, intref a,intref b)
    {
	const int result = a->internal_value() + b->internal_value();
	typespec int_spec = typespec("integer",{});
	fclass& int_cls = pContext->types().lookup(int_spec);
	objref pObject(new int_object(result,int_cls));
    
	return pObject;
    }

    objref in_range_integers(context* pContext, intref pThis, intref min, intref max)
    {
	bool in_range = (N_INT(pThis) >= N_INT(min)) && ( N_INT(pThis) <= N_INT(max));
	typespec bool_spec = typespec("boolean",{});
	fclass& bool_cls = pContext->types().lookup(bool_spec);
	objref pObject( new bool_object( (in_range)?true:false, bool_cls));
	return pObject;
    }

    objref logical_not(context* pContext,boolref a)
    {
	typespec bool_spec = typespec("boolean",{});
	fclass& bool_cls = pContext->types().lookup(bool_spec);
	objref pObject( new bool_object( !a->internal_value(), bool_cls ));
	return pObject;

    }
    
    objref list_size(context* pContext, listref pThis)
    {
	typespec int_spec = typespec("integer",{});
	fclass& int_cls = pContext->types().lookup(int_spec);
	
	objref pObject(new int_object(pThis->internal_value().size(),int_cls));
	return pObject;
    }

    objref list_head(context* pContext, listref pThis)
    {
	return pThis->first();
    }

    objref list_append(context* pContext, listref pThis, objref e)
    {
	pThis->append(e);
	return pThis;
    }

    objref list_tail(context* pContext, listref pThis)
    {
	return pThis->tail();
    }

    objref int_equate(context* pContext, intref pThis, intref pOther)
    {
	bool result = pThis->equate(pOther);
	typespec ts("boolean",{});
	return boolref(new bool_object(result,pContext->types().lookup(ts)));
    }
}
