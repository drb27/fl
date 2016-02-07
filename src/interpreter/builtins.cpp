#include <string>
#include <vector>
#include <cassert>
#include <random>
#include <deque>
#include "builtins.h"
#include <interpreter/class.h>
#include <interpreter/context.h>
#include <interpreter/object.h>
#include <parser/callable.h>
#include <parser/ast_nodes.h>
#include <logger/logger.h>
#include <interpreter/eval_exception.h>

using std::string;
using std::vector;
using std::deque;

#define N_INT(x) (x->internal_value())

namespace builtins
{
    void build_globals(context* pContext)
    {
	wlog_entry();
	typespec fnspec("function",{});
	fclass& fncls = pContext->types()->lookup(fnspec);

	deque<std::string> args;
	args.push_back("a"); 
	args.push_back("b");

	pContext->assign("rnd", 
			 fnref( new fn_object(pContext,fncls,
					      rawfn(make_marshall(&builtins::rnd)),
					      args,
					      {}) 
				) );

	pContext->assign("foreach",
			 fnref( new fn_object(pContext,fncls,
					      rawfn(make_marshall(&builtins::foreach)),
					      args,
					      {} ) ));
    }

    std::shared_ptr<fclass> object::build_class()
    {
	typespec spec("object",{});
	std::shared_ptr<fclass> pCls(new fclass(spec,nullptr));
	pCls->add_method( {"dump", make_marshall_mthd(&builtins::obj_dump),false});
	pCls->add_method( {"class", make_marshall_mthd(&builtins::obj_class)} );
	pCls->add_method( {".ctor", make_marshall_mthd(&builtins::obj_ctor),true});
	pCls->add_method( {".assign", make_marshall_mthd(&builtins::obj_assign),false});
	pCls->add_method( {"eq", make_marshall_mthd(&builtins::obj_equate),false} );
	pCls->add_method( {"is", make_marshall_mthd(&builtins::obj_is),true } );
	pCls->add_method( {"invoke", make_marshall_mthd(&builtins::obj_invoke),true} );
	return pCls;
    }

    std::shared_ptr<fclass> flclass::build_class(typemgr* pTm)
    {
	typespec base_spec("object",{});
	fclass& base_cls = pTm->lookup(base_spec);

	typespec spec("class",{});
	std::shared_ptr<fclass> pCls(new fclass(spec,&base_cls));

	pCls->add_method({"addmethod",make_marshall_mthd(&builtins::class_addmethod)});
	pCls->add_method({"methods",make_marshall_mthd(&builtins::class_methods)});
	pCls->add_method({"base",make_marshall_mthd(&builtins::class_base)});
	pCls->add_method({"derive",make_marshall_mthd(&builtins::class_derive),true});
	pCls->add_method({"new",make_marshall_mthd(&builtins::class_new),true});
	pCls->add_method({"addattr",make_marshall_mthd(&builtins::class_addattr),true});
	pCls->add_method({"eq", make_marshall_mthd(&builtins::class_equate),false} );
	pCls->add_method({"clsattrs", make_marshall_mthd(&builtins::class_attrlist)});

	return pCls;
    }

    std::shared_ptr<fclass> string::build_class(typemgr* pTm)
    {
	typespec base_spec("object",{});
	fclass& base_cls = pTm->lookup(base_spec);

	typespec spec("string",{});
	std::shared_ptr<fclass> pCls(new fclass(spec,&base_cls));
	pCls->add_method({"size",make_marshall_mthd(&builtins::string_length)});
	pCls->add_method({".index",make_marshall_mthd(&builtins::string_index)});
	pCls->add_method({"add",make_marshall_mthd(&builtins::string_add)});
	pCls->add_method({"join",make_marshall_mthd(&builtins::string_join)});
	return pCls;
    }

    std::shared_ptr<fclass> integer::build_class(typemgr* pTm)
    {
	typespec base_spec("object",{});
	fclass& base_cls = pTm->lookup(base_spec);

	typespec spec("integer",{});
	std::shared_ptr<fclass> pCls(new fclass(spec,&base_cls));
	pCls->add_method({"add", make_marshall_mthd(&builtins::add_integers)});
	pCls->add_method({"in_range", make_marshall_mthd(&builtins::in_range_integers)});
	pCls->add_method({"gt", make_marshall_mthd(&builtins::int_gt)});
	pCls->add_method({"lt", make_marshall_mthd(&builtins::int_lt)});
	pCls->add_method({"dec", make_marshall_mthd(&builtins::int_dec)});
	pCls->add_method({"div", make_marshall_mthd(&builtins::int_div)});
	pCls->add_method({"mod", make_marshall_mthd(&builtins::int_mod)});
	pCls->add_method({".float", make_marshall_mthd(&builtins::int_tofloat)});
	return pCls;
    }

    std::shared_ptr<fclass> flfloat::build_class(typemgr* pTm)
    {
	typespec base_spec("object",{});
	fclass& base_cls = pTm->lookup(base_spec);

	typespec spec("float",{});
	std::shared_ptr<fclass> pCls(new fclass(spec,&base_cls));
	pCls->add_method({"add", make_marshall_mthd(&builtins::float_add)});
	return pCls;
    }

    std::shared_ptr<fclass> flvoid::build_class(typemgr* pTm)
    {
	typespec base_spec("object",{});
	fclass& base_cls = pTm->lookup(base_spec);

	typespec spec("void",{});
	std::shared_ptr<fclass> pCls(new fclass(spec,&base_cls));
	return pCls;
    }

    std::shared_ptr<fclass> list::build_class(typespec spec, typemgr* pTm)
    {
	assert( spec.params().size()>0);

	typespec base_spec("object",{});
	typespec listobj_spec("list",{base_spec});

	if ( spec==listobj_spec)
	{
	    fclass& base_cls = pTm->lookup(base_spec);
	    std::shared_ptr<fclass> pCls(new fclass(spec,&base_cls));
	    pCls->add_method({"size", make_marshall_mthd(&builtins::list_size)});
	    pCls->add_method({"head", make_marshall_mthd(&builtins::list_head)});
	    pCls->add_method({"append", make_marshall_mthd(&builtins::list_append)});
	    pCls->add_method({"prepend", make_marshall_mthd(&builtins::list_prepend)});
	    pCls->add_method({"tail", make_marshall_mthd(&builtins::list_tail)});
	    pCls->add_method({"duplicate_and_append", make_marshall_mthd(&builtins::list_dup_and_append)});
	    pCls->add_method({"optimise", make_marshall_mthd(&builtins::list_optimise)});
	    pCls->add_method({"chunks", make_marshall_mthd(&builtins::list_chunks)});
	    pCls->add_method({"join", make_marshall_mthd(&builtins::list_join)});
	    pCls->add_method({".index", make_marshall_mthd(&builtins::list_index)});
	    pCls->add_method({".iter", make_marshall_mthd(&builtins::list_iter)});
	    pCls->add_method({"pop", make_marshall_mthd(&builtins::list_pop)});
	    return pCls;
	}
	else
	{
	    fclass& base_cls = pTm->lookup(listobj_spec);
	    std::shared_ptr<fclass> pCls(new fclass(spec,&base_cls));
	    return pCls;
	}
    }

    std::shared_ptr<fclass> function::build_class(typespec spec, typemgr* pTm)
    {
	typespec base_spec("object",{});
	fclass& base_cls = pTm->lookup(base_spec);
	std::shared_ptr<fclass> pCls(new fclass(spec,&base_cls));
	pCls->add_method({"itr", make_marshall_mthd(&builtins::fn_itr)});
	pCls->add_method({"name", make_marshall_mthd(&builtins::fn_name)});
	return pCls;
    }

    std::shared_ptr<fclass> boolean::build_class(typemgr* pTm)
    {
	typespec base_spec("object",{});
	fclass& base_cls = pTm->lookup(base_spec);

	typespec spec("boolean",{});
	std::shared_ptr<fclass> pCls(new fclass(spec,&base_cls));
	pCls->add_method({"not", make_marshall_mthd(&builtins::logical_not)});
	return pCls;
    }

    std::shared_ptr<fclass> flenum::build_class(typemgr* pTm)
    {
	typespec base_spec("object",{});
	fclass& base_cls = pTm->lookup(base_spec);

	typespec spec("enum",{});
	std::shared_ptr<fclass> pCls(new fclass(spec,&base_cls));
	pCls->add_class_method( {".iter", make_marshall_mthd(&builtins::enum_iter), false});
	pCls->add_method( {"str", make_marshall_mthd(&builtins::enum_str), false});
	return pCls;
    }

    objref add_integers(context* pContext, intref a, objref b)
    {
	// Check the type of b
	if ( &(b->get_class())!=&(a->get_class()) )
	    throw eval_exception(cerror::unsupported_argument,"Can't add this type to an integer");

	// Cast
	intref pB = std::dynamic_pointer_cast<int_object>(b);

	const int result = a->internal_value() + pB->internal_value();
	objref pObject(new int_object(pContext,result));
    
	return pObject;
    }

    objref in_range_integers(context* pContext, intref pThis, intref min, intref max)
    {
	bool in_range = (N_INT(pThis) >= N_INT(min)) && ( N_INT(pThis) <= N_INT(max));
	typespec bool_spec = typespec("boolean",{});
	fclass& bool_cls = pContext->types()->lookup(bool_spec);
	objref pObject( new bool_object(pContext, (in_range)?true:false, bool_cls));
	return pObject;
    }

    objref logical_not(context* pContext,boolref a)
    {
	typespec bool_spec = typespec("boolean",{});
	fclass& bool_cls = pContext->types()->lookup(bool_spec);
	objref pObject( new bool_object(pContext, !a->internal_value(), bool_cls ));
	return pObject;

    }
    
    objref list_size(context* pContext, listref pThis)
    {
	objref pObject(new int_object(pContext,pThis->size()));
	return pObject;
    }

    objref list_pop(context* pContext, listref pThis )
    {
	return pThis->pop();
    }

    objref list_head(context* pContext, listref pThis)
    {
	auto r = pThis->first();
	if (r)
	    return r;
	else
	{
	    typespec ts("void",{});
	    auto n = new void_object(pContext, pContext->types()->lookup(ts));
	    return objref(n);
	}
    }

    objref list_append(context* pContext, listref pThis, objref e)
    {
	pThis->append(e);
	return pThis;
    }

    objref list_prepend(context* pContext, listref pThis, objref e)
    {
	pThis->prepend(e);
	return pThis;
    }

    objref list_tail(context* pContext, listref pThis)
    {
	return pThis->tail(pContext);
    }

    objref list_iter(context* pContext, listref pThis )
    {
	return pThis;
    }

    objref int_gt(context* pContext, intref pThis, intref pOther )
    {
	bool result = pThis->internal_value() > pOther->internal_value();
	typespec ts("boolean",{});
	return boolref(new bool_object(pContext,result,pContext->types()->lookup(ts)));

    }

    objref int_lt(context* pContext, intref pThis, intref pOther )
    {
	bool result = pThis->internal_value() < pOther->internal_value();
	typespec ts("boolean",{});
	return boolref(new bool_object(pContext,result,pContext->types()->lookup(ts)));

    }

    objref int_dec(context* pContext,intref pThis)
    {
	return intref( new int_object(pContext, pThis->internal_value()-1));

	
    }

    objref int_div(context* pContext, intref pThis, intref divisor)
    {
	return intref( new int_object(pContext, N_INT(pThis)/N_INT(divisor)));

    }
    
    objref int_mod(context* pContext, intref pThis, intref modulus)
    {
	return intref( new int_object(pContext, N_INT(pThis)%N_INT(modulus)));
    }

    objref int_tofloat(context* pContext, intref pThis)
    {
	return floatref( new float_object(pContext, (double)N_INT(pThis)));
    }

    objref obj_dump(context*, objref pThis)
    {
	pThis->dump(std::cout);
	return pThis;
    }

    objref obj_class(context* pContext, objref pThis)
    {
	typespec ts("class",{});
	class_object* pClass = 
	    new class_object(pContext,&pThis->get_class(),pContext->types()->lookup(ts));
	return objref(pClass);
    }

    objref list_dup_and_append(context* pContext, listref pThis, objref pElement)
    {
	list_object* pNewList = new list_object(pContext, *pThis);
	pNewList->append(pElement);
	return objref(pNewList);
    }

    objref list_join(context* pContext, listref pThis, listref pOther)
    {
	list_object* pNewList = new list_object(pContext, *pThis);
	pNewList->append(pOther);
	return objref(pNewList);
    }

    objref string_length(context* pContext, stringref pThis )
    {
	// Create a new integer
	int_object* pResult = new int_object( pContext, pThis->internal_value().length() );

	return objref(pResult);
    }

    objref string_index(context* pContext, stringref pThis, intref index )
    {
	return (*pThis)[index];
    }

    objref string_add(context* pContext, stringref pThis, stringref pOther)
    {
	return stringref( (*pThis)+pOther );
    }

    objref string_join(context* pContext, stringref pThis, stringref pOther)
    {
	pThis->inplace_join(pOther);
	return pThis;
    }

    objref class_equate(context* pContext, objref pThis,objref pOther)
    {
	// Check they are both classes
	typespec tsc("class",{});
	fclass& cls = pContext->types()->lookup(tsc);
	bool result;
	if ( (&cls==&(pThis->get_class())) && (&cls==&(pOther->get_class())) )
	{
	    result = ( &(pThis->get_class()) == &(pOther->get_class()) );
	}
	else
	    result = false;
	// Calculate the result
	typespec ts("boolean",{});
	return boolref(new bool_object(pContext, result,pContext->types()->lookup(ts)));
    }

    objref class_methods(context* pContext, classref pThis)
    {
	// Typespecs
	typespec string_ts("string",{});
	typespec list_ts("list",{string_ts});
	fclass& string_cls = pContext->types()->lookup(string_ts);

	// Create a native list of string_objects
	std::list<objref> nativeList;

	// Add all the methods of the given class
	for ( auto m : pThis->internal_value()->methods() )
	{
	    string_object* pString = new string_object(pContext, m,string_cls);
	    nativeList.push_back( objref(pString) );
	}

	// Create a new list
	list_object* pList = new list_object( pContext, pContext->types()->lookup(list_ts),
					      nativeList);

	// return a managed reference
	return objref(pList);

    }

    objref class_attrlist(context* pContext, classref pThis )
    {
	// Typespecs
	typespec list_ts("list",{pThis->internal_value()->get_spec()});

	// Create a native list of string_objects
	std::list<objref> nativeList;

	// Add all the methods of the given class
	for ( auto m : pThis->internal_value()->class_attributes() )
	{
	    nativeList.push_back( m.second );
	}

	// Create a new list
	list_object* pList = new list_object( pContext, pContext->types()->lookup(list_ts),
					      nativeList);

	// return a managed reference
	return objref(pList);
	
    }

    objref class_base(context* pContext, classref pThis)
    {
	fclass* pInternalClass = pThis->internal_value();
	fclass* baseClass = pInternalClass->base();

	if ( pInternalClass->is_root() )
	{
	    // There is no base, return void
	    typespec ts("void",{});
	    return objref( new void_object(pContext, pContext->types()->lookup(ts)));
	}
	else
	{
	    // There is a base!
	    typespec spec("class",{});
	    return objref(new class_object(pContext, baseClass,pContext->types()->lookup(spec)));
	}
	
    }

    objref obj_ctor(context* pContext, objref pThis)
    {
	return pThis;
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
	pInternalClass->add_method({name->internal_value(), le});

	// Return a reference to the class object
	return pThis;
    }

    objref class_derive(context* pContext, classref pThis, stringref name)
    {
	typespec ts(name->internal_value(),{});
	typespec tsc("class",{});
	fclass* pNewNativeClass = new fclass(ts,pThis->internal_value());
	(pContext->types())->add(*pNewNativeClass);
	class_object*  pNewClass = 
	    new class_object(pContext, pNewNativeClass,(pContext->types())->lookup(tsc));

	return objref(pNewClass,[](class_object*){});
    }

    objref class_new(context* pContext, classref pThis, listref params)
    {
	vector<objref> evaled_params;
	//for ( auto o : params->internal_value() )
	for ( int index=0; index < params->size() ; index++ )
	{
	    evaled_params.push_back(params->get_element(index));
	}
	::object* pObj = new ::object(pContext, *(pThis->internal_value()),evaled_params);
	return objref(pObj);
    }

    objref class_addattr(context* pContext, classref pThis, stringref name, objref d)
    {
	fclass* pNativeClass = pThis->internal_value();
	pNativeClass->add_attribute(name->internal_value(),d);
	return pThis;
    }

    objref obj_assign(context* pContext, objref pThis, objref pOther)
    {
	(*pThis)=pOther;
    }
    
    objref obj_equate(context* pContext, objref pThis,objref pOther)
    {
	bool result = (*pThis)==pOther;
	typespec ts("boolean",{});
	return boolref(new bool_object(pContext, result,pContext->types()->lookup(ts)));
    }

    objref rnd(context* pContext, intref a, intref b)
    {
	int lower = a->internal_value();
	int upper = b->internal_value();

	static std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(lower,upper);

	return objref( new int_object(pContext, distribution(generator)));
    }

    objref foreach(context* pContext, objref pObj, fnref pFn )
    {
	// Call .iter on the object to yield a list object
	auto m = methodcall_node(".iter");
	ast* t = new literal_node(pObj);
	m.add_target(t);
	listref l = std::dynamic_pointer_cast<list_object>( m.evaluate(pContext) );
	delete t;

	// We now have a list of things to apply the function to.
	typespec tso("object",{});
	typespec tsl("list", { tso} );
	auto& list_cls = pContext->types()->lookup(tsl);
	listref returnList = listref( new list_object(pContext,list_cls) );

	for ( int index=0; index<l->size(); index++ )
	{
	    objref currentObject = l->get_element(index);
	    
	    // Apply the function
	    list_node* pArgList = new list_node();
	    literal_node* pArg = new literal_node(currentObject);
	    pArgList->push_element( pArg );
	    funcall_node* pFnCall = new funcall_node("(anonymous)", pArgList);

	    auto result = pFnCall->evaluate(pContext,pFn);

	    // Add the result of the function to the result list
	    returnList->append(result);

	    delete pFnCall;
	    delete pArg;
	    delete pArgList;
	    
	}

	return returnList;
    }

    objref obj_is(context* pContext,objref pThis, objref pOther)
    {
	bool result = pThis.get()==pOther.get();
	typespec ts("boolean",{});
	return boolref(new bool_object(pContext,result,pContext->types()->lookup(ts)));
    }

    objref obj_invoke(context* pContext,objref pThis,stringref name, listref params)
    {
	vector<objref> raw_params(params->size());

	//for ( auto p : params->internal_value() )
	for ( int index=0; index < params->size(); index++ )
	{
	    raw_params[index] = params->get_element(index);
	}
	return pThis->invoke( name->internal_value(), pContext, raw_params );
    }
    
    objref list_optimise(context* pContext, listref pThis )
    {
	pThis->optimise();
	return pThis;
    }

    objref list_chunks(context* pContext, listref pThis )
    {
	int_object* i = new int_object(pContext, pThis->chunks() );
	return objref(i);
    }

    objref list_index(context* pContext, listref pThis, objref i )
    {
	typespec intspec("integer",{});
	if ( &(i->get_class())!=&(pContext->types()->lookup(intspec)) )
	{
	    throw eval_exception( cerror::invalid_index, "Unsupported index type");
	}

	int index = N_INT(std::dynamic_pointer_cast<int_object>(i)); 
	if (  index < pThis->size() )
	{
	    return pThis->get_element(index);
	}
	else
	{
	    throw eval_exception( cerror::index_out_of_bounds, "Index out of bounds" );
	}

    }
    
    objref fn_itr(context* pContext, fnref pThis )
    {
	typespec bs("boolean",{});
	bool_object* r = new bool_object(pContext, pThis->is_tail_recursive(), pContext->types()->lookup(bs) );
	return boolref(r);
    }

    objref fn_name(context* pContext, fnref pThis )
    {
	typespec ts("string",{});
	string_object* pString = new string_object(pContext, 
						   pThis->name(),
						   pContext->types()->lookup(ts) );

	return stringref(pString);
    }

    objref enum_iter(context* pContext, classref pThis )
    {
	typespec tsl("list", {pThis->internal_value()->get_spec()} );
	auto& list_cls = pContext->types()->lookup(tsl);
	listref returnList = listref( new list_object(pContext,list_cls) );
	
	for( auto e : pThis->class_attributes() )
	{
	    returnList->append(e.second);
	}

	return returnList;
    }

    objref enum_str(context* pContext, enumref pThis )
    {
	return pThis->str();
    }

    objref float_add(context* pContext, floatref a, objref b)
    {
	// Check the type of b
	if ( &(b->get_class())!=&(a->get_class()) )
	    throw eval_exception(cerror::unsupported_argument,"Can't add this type to a float");

	// Cast
	floatref pB = std::dynamic_pointer_cast<float_object>(b);

	const double result = a->internal_value() + pB->internal_value();
	objref pObject(new float_object(pContext,result));
    
	return pObject;
    }

}
