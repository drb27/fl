#include <string>
#include <vector>
#include "builtins.h"
#include <interpreter/class.h>
#include <interpreter/context.h>
#include <parser/callable.h>
#include <parser/ast_nodes.h>
#include <logger/logger.h>

using std::string;
using std::vector;

#define N_INT(x) (x->internal_value())

namespace builtins
{
    std::shared_ptr<fclass> object::build_class()
    {
	typespec spec("object",{});
	std::shared_ptr<fclass> pCls(new fclass(spec));
	pCls->add_method("dump", make_marshall_mthd(&builtins::obj_dump));
	pCls->add_method("class", make_marshall_mthd(&builtins::obj_class));
	return pCls;
    }

    std::shared_ptr<fclass> flclass::build_class()
    {
	typespec spec("class",{});
	std::shared_ptr<fclass> pCls(new fclass(spec));
	pCls->add_method("addmethod",make_marshall_mthd(&builtins::class_addmethod));
	return pCls;
    }

    std::shared_ptr<fclass> string::build_class()
    {
	typespec spec("string",{});
	std::shared_ptr<fclass> pCls(new fclass(spec));
	pCls->add_method("size",make_marshall_mthd(&builtins::string_length));
	return pCls;
    }

    std::shared_ptr<fclass> integer::build_class(typemgr* pTm)
    {
	typespec base_spec("object",{});
	const fclass& base_cls = pTm->lookup(base_spec);

	typespec spec("integer",{});
	std::shared_ptr<fclass> pCls(new fclass(spec,base_cls));
	pCls->add_method("add", make_marshall_mthd(&builtins::add_integers));
	pCls->add_method("in_range", make_marshall_mthd(&builtins::in_range_integers));
	pCls->add_method("eq", make_marshall_mthd(&builtins::int_equate));
	pCls->add_method("gt", make_marshall_mthd(&builtins::int_gt));
	pCls->add_method("dec", make_marshall_mthd(&builtins::int_dec));
	return pCls;
    }

    std::shared_ptr<fclass> flvoid::build_class()
    {
	typespec spec("void",{});
	std::shared_ptr<fclass> pCls(new fclass(spec,true));
	return pCls;
    }

    std::shared_ptr<fclass> list::build_class(typespec spec, typemgr* pTm)
    {
	typespec base_spec("object",{});
	const fclass& base_cls = pTm->lookup(base_spec);

	std::shared_ptr<fclass> pCls(new fclass(spec,base_cls));
	pCls->add_method("size", make_marshall_mthd(&builtins::list_size));
	pCls->add_method("head", make_marshall_mthd(&builtins::list_head));
	pCls->add_method("append", make_marshall_mthd(&builtins::list_append));
	pCls->add_method("tail", make_marshall_mthd(&builtins::list_tail));
	pCls->add_method("duplicate_and_append", make_marshall_mthd(&builtins::list_dup_and_append));
	return pCls;
    }

    std::shared_ptr<fclass> function::build_class(typespec spec, typemgr* pTm)
    {
	typespec base_spec("object",{});
	const fclass& base_cls = pTm->lookup(base_spec);

	std::shared_ptr<fclass> pCls(new fclass(spec,base_cls));
	return pCls;
    }

    std::shared_ptr<fclass> boolean::build_class(typemgr* pTm)
    {
	typespec base_spec("object",{});
	const fclass& base_cls = pTm->lookup(base_spec);

	typespec spec("boolean",{});
	std::shared_ptr<fclass> pCls(new fclass(spec,base_cls));
	pCls->add_method("not", make_marshall_mthd(&builtins::logical_not));
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

    objref int_gt(context* pContext, intref pThis, intref pOther )
    {
	bool result = pThis->internal_value() > pOther->internal_value();
	typespec ts("boolean",{});
	return boolref(new bool_object(result,pContext->types().lookup(ts)));

    }

    objref int_dec(context* pContext,intref pThis)
    {
	typespec ts("integer",{});
	return intref( new int_object(pThis->internal_value()-1,
				      pContext->types().lookup(ts)));
	
    }

    objref obj_dump(context*, objref pThis)
    {
	pThis->dump(std::cout);
	return pThis;
    }

    objref obj_class(context* pContext, objref pThis)
    {
	typespec ts("class",{});
	class_object* pClass = new class_object(&pThis->get_class(),pContext->types().lookup(ts));
	return objref(pClass);
    }

    objref list_dup_and_append(context* pContext, listref pThis, objref pElement)
    {
	list_object* pNewList = new list_object(pThis->get_class(),
						pThis->internal_value() );

	pNewList->append(pElement);
	return objref(pNewList);
    }

    objref string_length(context* pContext, stringref pThis )
    {
	// Create a new integer
	typespec ts("integer",{});
	int_object* pResult = new int_object( pThis->internal_value().length(),
					      pContext->types().lookup(ts));
	return objref(pResult);
    }

    objref class_addmethod(context* pContext, classref pThis, fnref  fn, stringref name)
    {
	// Construct a lambda which executes the method on the object
	auto le = [fn](context* pContext, objref pThis, std::vector<ast*>& params)
	    {
		// Evaluate each parameter,ignoring the first two
		vector<objref> evaled_params;
		evaled_params.push_back(pThis);
		int guard=0;
		for ( auto arg : params )
		{
		    if (guard++>1)
			evaled_params.push_back( arg->evaluate(pContext) );
		}
		return (*fn)(pContext,evaled_params);
	    };

	// Add the method to the class
	fclass* pInternalClass = pThis->internal_value();
	pInternalClass->add_method(name->internal_value(), le);

	// Return a reference to the class object
	return pThis;
    }
}
