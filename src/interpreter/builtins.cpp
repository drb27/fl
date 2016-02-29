#include <string>
#include <vector>
#include <cassert>
#include <random>
#include <deque>
#include <set>
#include <functional>
#include <sstream>
#include "builtins.h"
#include <interpreter/class.h>
#include <interpreter/context.h>
#include <parser/callable.h>
#include <parser/ast/methodcall.h>
#include <parser/ast/literal.h>
#include <parser/ast/list.h>
#include <parser/ast/funcall.h>
#include <logger/logger.h>
#include <interpreter/eval_exception.h>
#include <parser/rawfn.h>
#include <interpreter/obj/all.h>
#include <interpreter/factory.h>

using std::function;
using std::string;
using std::vector;
using std::deque;
using std::set;
using std::pair;
using std::stringstream;

#define N_INT(x) (x->internal_value())
#define N_BOOL(x) (x->internal_value())
#define N_CLASS(x) (x->internal_value())
#define N_FLOAT(x) (x->internal_value())

namespace builtins
{

    fclass* object::_class{nullptr};
    fclass* flclass::_class{nullptr};
    fclass* integer::_class{nullptr};
    fclass* flfloat::_class{nullptr};
    fclass* function::_class{nullptr};
    fclass* flvoid::_class{nullptr};
    fclass* boolean::_class{nullptr};
    fclass* string::_class{nullptr};
    fclass* flenum::_class{nullptr};
    fclass* list::_class{nullptr};
    fclass* lazy::_class{nullptr};
    fclass* signal::_class{nullptr};
    fclass* eval_signal::_class{nullptr};

    fclass* object::get_class()
    {
	if (!_class)
	    _class = object::build_class();

	return _class;
    }

    fclass* list::get_class()
    {
	if (!_class)
	    _class = list::build_class();

	return _class;
    }

    fclass* flclass::get_class()
    {
	if (!_class)
	    _class = flclass::build_class();

	return _class;
    }

    fclass* integer::get_class()
    {
	if (!_class)
	    _class = integer::build_class();

	return _class;
    }

    fclass* lazy::get_class()
    {
	if (!_class)
	    _class = lazy::build_class();

	return _class; 
    }
    
    fclass* signal::get_class()
    {
	if (!_class)
	    _class = signal::build_class();

	return _class;
    }

    fclass* eval_signal::get_class()
    {
	if (!_class)
	    _class = eval_signal::build_class();

	return _class;
    }

    fclass* flfloat::get_class()
    {
	if (!_class)
	    _class = flfloat::build_class();

	return _class;
    }

    fclass* function::get_class()
    {
	if (!_class)
	    _class = function::build_class();

	return _class;
    }

    fclass* flvoid::get_class()
    {
	if (!_class)
	    _class = flvoid::build_class();

	return _class;
    }

    fclass* boolean::get_class()
    {
	if (!_class)
	    _class = boolean::build_class();

	return _class;
    }

    fclass* string::get_class()
    {
	if (!_class)
	    _class = string::build_class();

	return _class;
    }

    fclass* flenum::get_class()
    {
	if (!_class)
	    _class = flenum::build_class();

	return _class;
    }

    void build_globals(context* pContext)
    {
	wlog_entry();

	fn_object::hinted_args_t args;
	args.push_back({"a",nullptr}); 
	args.push_back({"b",nullptr});

	pContext->assign(std::string("rnd"), 
			 fnref( new fn_object(pContext,
					      rawfn(make_marshall(&builtins::rnd)),
					      args,args,
					      {}) 
				) );

	pContext->assign(std::string("foreach"),
			 fnref( new fn_object(pContext,
					      rawfn(make_marshall(&builtins::foreach)),
					      args,args,
					      {} ) ));

	args.pop_back();
	pContext->assign(std::string("I"), 
			 fnref( new fn_object(pContext,
					      rawfn(make_marshall(&builtins::I)),
					      args,args,
					      {}) 
				) );

	pContext->assign(std::string("print"), 
			 fnref( new fn_object(pContext,
					      rawfn(make_marshall(&builtins::print)),
					      args,args,
					      {}) 
				) );
   
	pContext->assign( integer::get_class()->name(), 
			  classref(new class_object(pContext, integer::get_class())) );

	pContext->assign( lazy::get_class()->name(), 
			  classref(new class_object(pContext, lazy::get_class())) );

	pContext->assign( signal::get_class()->name(), 
			  classref(new class_object(pContext, signal::get_class())) );

	pContext->assign( eval_signal::get_class()->name(), 
			  classref(new class_object(pContext, eval_signal::get_class())) );

	pContext->assign( flfloat::get_class()->name(), 
			  classref(new class_object(pContext, flfloat::get_class())) );

	pContext->assign( string::get_class()->name(), 
			  classref(new class_object(pContext, string::get_class())) );

	pContext->assign( flvoid::get_class()->name(), 
			  classref(new class_object(pContext, flvoid::get_class())) );

	pContext->assign( list::get_class()->name(), 
			  classref(new class_object(pContext, list::get_class())) );

	pContext->assign( boolean::get_class()->name(), 
			  classref(new class_object(pContext, boolean::get_class())) );

	pContext->assign( flclass::get_class()->name(), 
			  classref(new class_object(pContext, flclass::get_class())) );

	pContext->assign( function::get_class()->name(), 
			  classref(new class_object(pContext, function::get_class())) );

	pContext->assign( flenum::get_class()->name(), 
			  classref(new class_object(pContext, flenum::get_class())) );

	pContext->assign( object::get_class()->name(), 
			  classref(new class_object(pContext, object::get_class())) );

    }

    fclass* object::build_class()
    {
	typespec spec("object");
	fclass* pCls = new fclass(spec,nullptr,false,true,false);
	
	ctorinfo c;
	c.name="<constructor>";
	c.fn = make_marshall_mthd(&builtins::obj_ctor);
	pCls->set_ctor(c);

	factory::get().add_spawner( pCls, [](context* ctx, fclass* cls) 
				    { 
					return objref(new ::object(ctx,*cls));
				    } );

	pCls->add_method( {"dump", make_marshall_mthd(&builtins::obj_dump),false});
	pCls->add_method( {"class", make_marshall_mthd(&builtins::obj_class)} );
	pCls->add_method( {".assign", make_marshall_mthd(&builtins::obj_assign),false});
	pCls->add_method( {"eq", make_marshall_mthd(&builtins::obj_equate),false} );
	pCls->add_method( {".selmatch", make_marshall_mthd(&builtins::obj_selmatch),false} );
	pCls->add_method( {"is", make_marshall_mthd(&builtins::obj_is),true } );
	pCls->add_method( {"invoke", make_marshall_mthd(&builtins::obj_invoke),true} );
	pCls->add_method( {"can_convert", make_marshall_mthd(&builtins::obj_convertible_to),true} );	
	pCls->add_method( {"convert", make_marshall_mthd(&builtins::obj_convert),true} );
	return pCls;
    }

    fclass* flclass::build_class()
    {
	fclass* base_cls = object::get_class();
	typespec spec("class");

	fclass* pCls = new fclass(spec,base_cls,false,true,false,true);
	pCls->add_method({"addmethod",make_marshall_mthd(&builtins::class_addmethod)});
	pCls->add_method({"constructor",make_marshall_mthd(&builtins::class_constructor)});
	pCls->add_method({"methods",make_marshall_mthd(&builtins::class_methods)});
	pCls->add_method({"attributes",make_marshall_mthd(&builtins::class_attributes)});
	pCls->add_method({"base",make_marshall_mthd(&builtins::class_base)});
	pCls->add_method({"new",make_marshall_mthd(&builtins::class_new),true});
	pCls->add_method({"addattr",make_marshall_mthd(&builtins::class_addattr),true});
	pCls->add_method({"eq", make_marshall_mthd(&builtins::class_equate),false} );
	pCls->add_method({"clsattrs", make_marshall_mthd(&builtins::class_attrlist)});

	return pCls;
    }

    fclass* string::build_class()
    {
	fclass* base_cls = object::get_class();
	typespec spec("string");

	fclass* pCls = new fclass(spec,base_cls,false,true,false,true);
	pCls->add_method({"size",make_marshall_mthd(&builtins::string_length)});
	pCls->add_method({".index",make_marshall_mthd(&builtins::string_index)});
	pCls->add_method({"add",make_marshall_mthd(&builtins::string_add)});
	pCls->add_method({"join",make_marshall_mthd(&builtins::string_join)});
	return pCls;
    }

    fclass* integer::build_class()
    {
	fclass* base_cls = object::get_class();
	typespec spec("integer");
	fclass* pCls = new fclass(spec,base_cls,false,true,true,false);

	ctorinfo c;
	c.name="<constructor>";
	c.fn = make_marshall_mthd(&builtins::int_ctor);
	c.args.push_back("x");
	pCls->set_ctor(c);

	factory::get().add_spawner( pCls, [](context* ctx, fclass* cls) 
				    { 
					return objref(new ::int_object(ctx,0,*cls));
				    } );

	pCls->add_method({"add", make_marshall_mthd(&builtins::add_integers)});
	pCls->add_method({"in_range", make_marshall_mthd(&builtins::in_range_integers)});
	pCls->add_method({"gt", make_marshall_mthd(&builtins::int_gt)});
	pCls->add_method({"lt", make_marshall_mthd(&builtins::int_lt)});
	pCls->add_method({"dec", make_marshall_mthd(&builtins::int_dec)});
	pCls->add_method({"div", make_marshall_mthd(&builtins::int_div)});
	pCls->add_method({"divf", make_marshall_mthd(&builtins::int_divf)});
	pCls->add_method({"mod", make_marshall_mthd(&builtins::int_mod)});
	pCls->add_method({"->float", make_marshall_mthd(&builtins::int_tofloat)});
	pCls->add_method({"->boolean", make_marshall_mthd(&builtins::int_to_bool)});
	pCls->add_method({"->string", make_marshall_mthd(&builtins::int_to_string)});
	return pCls;
    }

    fclass* lazy::build_class()
    {
	fclass* base_cls = object::get_class();
	typespec spec("lazy");
	fclass* pCls = new fclass(spec,base_cls,false,true,true,false);

	ctorinfo c;
	c.name="<constructor>";
	c.fn = make_marshall_mthd(&builtins::lazy_ctor);
	pCls->set_ctor(c);

	factory::get().add_spawner( pCls, [](context* ctx, fclass* cls) 
				    { 
					return objref(new ::lazy_object(ctx,nullptr,*cls));
				    } );

	pCls->add_method({"evaluate", make_marshall_mthd(&builtins::lazy_evaluate)});
	return pCls;

    }

    fclass* signal::build_class()
    {
	fclass* base_cls = object::get_class();
	typespec spec("signal");
	fclass* pCls = new fclass(spec,base_cls,false,true,true,false);

	ctorinfo c;
	c.name="<constructor>";
	c.fn = make_marshall_mthd(&builtins::signal_ctor);
	pCls->set_ctor(c);

	factory::get().add_spawner( pCls, [](context* ctx, fclass* cls) 
				    { 
					return objref(new ::signal_object(ctx,*cls));
				    } );

	return pCls;
    }
    fclass* eval_signal::build_class()
    {
	fclass* base_cls = signal::get_class();
	typespec spec("evalsignal");
	fclass* pCls = new fclass(spec,base_cls,false,true,true,false);

	ctorinfo c;
	c.name="<constructor>";
	c.fn = make_marshall_mthd(&builtins::eval_signal_ctor);
	pCls->set_ctor(c);

	factory::get().add_spawner( pCls, [](context* ctx, fclass* cls) 
				    { 
					return objref(new ::eval_signal_object(ctx,nullptr,*cls));
				    } );

	return pCls;
    }

    fclass* flfloat::build_class()
    {
	fclass* base_cls = object::get_class();
	typespec spec("float");

	fclass* pCls = new fclass(spec,base_cls,false,true,false,true);
	pCls->add_method({"add", make_marshall_mthd(&builtins::float_add)});
	pCls->add_method({"->integer", make_marshall_mthd(&builtins::float_to_int)});
	return pCls;
    }

    fclass* flvoid::build_class()
    {
	fclass* base_cls = object::get_class();
	typespec spec("void");

	fclass* pCls = new fclass(spec,base_cls,false,true,false,true);
	return pCls;
    }

    fclass* list::build_class()
    {
	typespec spec("list");
	fclass* base_cls = object::get_class();

	fclass* pCls = new fclass(spec,base_cls,false,true,false);
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

    fclass* function::build_class()
    {
	typespec spec("function");
	fclass* base_cls = object::get_class();

	fclass* pCls = new fclass(spec,base_cls,false,true,false,true);
	pCls->add_method({"itr", make_marshall_mthd(&builtins::fn_itr)});
	pCls->add_method({"name", make_marshall_mthd(&builtins::fn_name)});
	return pCls;
    }

    fclass* boolean::build_class()
    {
	fclass* base_cls = object::get_class();
	typespec spec("boolean");

	fclass* pCls = new fclass(spec,base_cls,false,true,false,true);
	pCls->add_method({"not", make_marshall_mthd(&builtins::logical_not)});
	pCls->add_method({"->integer", make_marshall_mthd(&builtins::bool_to_int)});
	return pCls;
    }

    fclass* flenum::build_class()
    {
	fclass* base_cls = object::get_class();
	typespec spec("enum");

	fclass* pCls = new fclass(spec,base_cls,true,true,false,true);
	pCls->add_class_method( {".iter", make_marshall_mthd(&builtins::enum_iter), false});
	pCls->add_method( {"->string", make_marshall_mthd(&builtins::enum_str), false});
	pCls->add_method( {"->integer", make_marshall_mthd(&builtins::enum_toint), false});
	pCls->add_method( {"toint", make_marshall_mthd(&builtins::enum_toint), false});
	pCls->add_method( {"tostr", make_marshall_mthd(&builtins::enum_str), false});
	return pCls;
    }

    objref add_integers(context* pContext, intref a, objref b)
    {
	b = ::object::convert_to(b,builtins::integer::get_class());

	// Check the result of the conversion
	if (!b)
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
	return objref( new bool_object(pContext, (in_range)?true:false));
    }

    objref logical_not(context* pContext,boolref a)
    {
	return objref( new bool_object(pContext, !a->internal_value() )); 
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
	    return objref(new void_object(pContext));
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
	return boolref(new bool_object(pContext, result));
    }

    objref int_lt(context* pContext, intref pThis, intref pOther )
    {
	bool result = pThis->internal_value() < pOther->internal_value();
	return boolref(new bool_object(pContext, result));
    }

    objref int_dec(context* pContext,intref pThis)
    {
	return intref( new int_object(pContext, pThis->internal_value()-1));
    }

    objref int_to_string(context* pContext, intref pThis )
    {
	stringstream s;
	s << (pThis->internal_value());
	return objref( new string_object(pContext, s.str() ) );
    }
    
    objref int_ctor(context* pContext, intref pThis, objref pOther)
    {
	auto o = ::object::convert_to(pOther,builtins::integer::get_class());

	// Check the result of the conversion
	if (!o)
	    throw eval_exception(cerror::unsupported_argument,"Can't construct an integer from this type");

	auto oInt = ::object::cast_or_abort<int_object>(o);
	pThis->set_internal_value( oInt->internal_value() );
	return pThis;
    }

    objref lazy_ctor(context*,lazyref pThis)
    {
	return pThis;
    }

    objref signal_ctor(context*,sigref pThis)
    {
	return pThis;
    }

    objref eval_signal_ctor(context*,evalsigref pThis)
    {
	return pThis;
    }

    objref lazy_evaluate(context* pContext,lazyref pThis)
    {
	return pThis->internal_value()->evaluate(pContext);
    }
    
    objref int_div(context* pContext, intref pThis, intref divisor)
    {
	return intref( new int_object(pContext, N_INT(pThis)/N_INT(divisor)));
    }

    objref int_divf(context* pContext, intref pThis, objref divisor)
    {

	// Try float first - if it is already a float, then good, if it is an int, 
	// then conversion to float does not harm
	divisor = ::object::convert_to(divisor,builtins::flfloat::get_class() );

	if (!divisor)
	    throw eval_exception(cerror::unsupported_argument,"Can't divf() this type");
	else
	{
	    floatref fd = std::dynamic_pointer_cast<float_object>(divisor);
	    return floatref( new float_object(pContext, 
					      ((double)N_INT(pThis))/((double)N_FLOAT(fd))));	    
	}


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
	return objref(new class_object(pContext, &pThis->get_class() ));
    }

    objref obj_convertible_to(context* pContext, objref pThis, classref pTargetClass)
    {
	fclass& startClass = pThis->get_class();
	fclass& targetClass = *(N_CLASS(pTargetClass));

	bool returnVal = startClass.can_convert_to(&targetClass);
	return boolref( new bool_object(pContext, returnVal ));

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

    objref string_add(context* pContext, stringref pThis, objref pOther)
    {
	pOther = ::object::convert_to(pOther,builtins::string::get_class());

	// Check the result of the conversion
	if (!pOther)
	    throw eval_exception(cerror::unsupported_argument,
				 "Attempt to add a non-string to a string, and can't convert");

	// Cast
	stringref pB = ::object::cast_or_abort<string_object>(pOther);

	return stringref( (*pThis)+pB);
    }

    objref string_join(context* pContext, stringref pThis, stringref pOther)
    {
	pThis->inplace_join(pOther);
	return pThis;
    }

    objref class_equate(context* pContext, objref pThis,objref pOther)
    {
	// Check they are both classes
	auto cls = builtins::flclass::get_class();
	bool result;

	if ( (cls==&(pThis->get_class())) && (cls==&(pOther->get_class())) )
	{
	    auto pThisCls = std::dynamic_pointer_cast<class_object>(pThis);
	    auto pOtherCls = std::dynamic_pointer_cast<class_object>(pOther);

	    result = ( (pThisCls->internal_value()) == (pOtherCls->internal_value()) );
	}
	else
	    result = false;

	// Calculate the result
	return boolref(new bool_object(pContext, result ));
    }

    objref class_methods(context* pContext, classref pThis)
    {
	// Create a native list of string_objects
	std::list<objref> nativeList;

	set<std::string> strMethods;
	pThis->internal_value()->all_methods(strMethods);
	// Add all the methods of the given class
	for ( auto m : strMethods )
	{
	    string_object* pString = new string_object(pContext, m);
	    nativeList.push_back( objref(pString) );
	}

	// Create a new list
	return objref( new list_object(pContext, nativeList) );
    }

    objref class_attributes(context* pContext, classref pThis)
    {
	// Create a native list of string_objects
	std::list<objref> nativeList;

	set<std::string> strAttrs;
	pThis->internal_value()->all_attributes(strAttrs);
	// Add all the attributes of the given class
	for ( auto m : strAttrs )
	{
	    string_object* pString = new string_object(pContext, m);
	    nativeList.push_back( objref(pString) );
	}

	// Create a new list
	return objref( new list_object(pContext, nativeList) );
    }

    objref class_attrlist(context* pContext, classref pThis )
    {

	// Create a native list of string_objects
	std::list<objref> nativeList;

	// Add all the methods of the given class
	for ( auto m : pThis->internal_value()->class_attributes() )
	{
	    nativeList.push_back( m.second );
	}

	// Create a new list
	return objref( new list_object(pContext, nativeList) );
    }

    objref class_base(context* pContext, classref pThis)
    {
	fclass* pInternalClass = pThis->internal_value();
	fclass* baseClass = pInternalClass->base();

	if ( pInternalClass->is_root() )
	{
	    // There is no base, return void
	    return objref(new void_object(pContext));
	}
	else
	{
	    // There is a base!
	    return objref(new class_object(pContext, baseClass));
	}
	
    }

    objref obj_ctor(context* pContext, objref pThis)
    {
	return pThis;
    }

    objref class_addctor(context* pContext, classref pThis, fnref  fn, listref chain)
    {
	//TODO: #43 Implement class_addctor
	return objref(nullptr);
    }

    std::function<marshall_mthd_t> make_method_lambda( fnref fn )
    {
	return [fn](context* pContext, objref pThis, std::vector<ast*>& params)
	{
	    auto class_cls = builtins::flclass::get_class();

	    // Evaluate each parameter,ignoring the first two
	    vector<objref> evaled_params;
	    evaled_params.push_back(pThis);
	    int guard=0;
	    auto& argList = fn->arglist();
	    auto full_i = argList.begin();
	    full_i++; // Skip the first, it is the class
	    for ( auto arg : params )
	    {
		if (guard++>1)
		{
		    if ( (*full_i).second)
		    {
			objref pHintedObj = (*full_i).second->evaluate(pContext);
			if ( &(pHintedObj->get_class()) == class_cls )
			{
			    classref pHintedCls = std::dynamic_pointer_cast<class_object>(pHintedObj);
			    evaled_params.push_back( ::object::convert_to( arg->evaluate(pContext),
									   pHintedCls->internal_value() ));
			}
			else
			{
			    throw eval_exception( cerror::not_a_class,
						  "Type hint does not evaluate to a class object" );
			}    
		    }
		    else
			evaled_params.push_back( arg->evaluate(pContext) );
			
		    full_i++;
		}

	    }

	    return (*fn)(pContext,evaled_params);
	};
    }
    
    objref class_addmethod(context* pContext, classref pThis, fnref  fn, stringref name)
    {
	// Construct a lambda which executes the method on the object
	auto le = make_method_lambda(fn); 

	// Add the method to the class
	fclass* pInternalClass = pThis->internal_value();
	pInternalClass->add_method({name->internal_value(), le});

	// Return a reference to the class object
	return pThis;
    }

    objref class_new(context* pContext, classref pThis, listref params)
    {
	fclass* const pTargetClass = N_CLASS(pThis);

	if ( !pTargetClass->allow_new() )
	{
	    throw eval_exception(cerror::no_construction,
				 "Explicit construction via new() is prohibited for this class" );
	}

	return factory::get().make_object(pContext,pTargetClass,params);
    }

    objref class_constructor(context* pContext, classref pThis, fnref pCtor, lazyref pChain )
    {
	fclass* const pTargetClass = N_CLASS(pThis);

	ctorinfo c;
	c.name = "<constructor>";
	c.chain_params = pChain;
	c.args.clear();

	// Construct a lambda which executes the method on the object
	c.fn = make_method_lambda(pCtor);
	
	for ( auto hinted_arg : pCtor->arglist() )
	{
	    c.args.push_back( hinted_arg.first );
	}

	// Register the constructor with the class
	pThis->internal_value()->set_ctor(c);

	return pThis;
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
    
    objref obj_selmatch(context* pContext, objref pThis,objref pOther)
    {
	bool result = pThis->selector_match(pOther);
	return boolref(new bool_object(pContext, result));
    }
    
    objref obj_equate(context* pContext, objref pThis,objref pOther)
    {
	bool result = (*pThis)==pOther;
	return boolref(new bool_object(pContext, result));
    }
    
    objref print(context* pContext, objref a)
    {
	a = ::object::convert_to(a,builtins::string::get_class());

	// Check the result of the conversion
	if (!a)
	    throw eval_exception(cerror::unsupported_argument,
				 "Argument to print must be a string, or convertible to a string");

	stringref str = ::object::cast_or_abort<string_object>(a);
	std::cout << ( str->internal_value() ) << std::endl;

	return a;
    }
    
    objref rnd(context* pContext, intref a, intref b)
    {
	int lower = a->internal_value();
	int upper = b->internal_value();

	static std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(lower,upper);

	return objref( new int_object(pContext, distribution(generator)));
    }
    
    objref I(context* pContext, objref a)
    {
	return a;
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
	listref returnList = listref( new list_object(pContext) );

	for ( int index=0; index<l->size(); index++ )
	{
	    objref currentObject = l->get_element(index);
	    
	    // Apply the function
	    list_node* pArgList = new list_node();
	    literal_node* pArg = new literal_node(currentObject);
	    pArgList->push_element( pArg );
	    funcall_node* pFnCall = new funcall_node(symspec("(anonymous)"), 
						     pArgList,
						     pFn);

	    auto result = pFnCall->evaluate(pContext);

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
	return boolref(new bool_object(pContext,result));
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
	if ( &(i->get_class())!= builtins::integer::get_class() )
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
	return objref(new bool_object(pContext, pThis->is_tail_recursive()) );
    }

    objref fn_name(context* pContext, fnref pThis )
    {
	return stringref(new string_object(pContext, pThis->name() ));
    }

    objref enum_iter(context* pContext, classref pThis )
    {
	listref returnList = listref( new list_object(pContext) );
	
	for( auto e : pThis->class_attributes() )
	{
	    returnList->append(e.second);
	}

	return returnList;
    }

    objref enum_toint(context* pContext, enumref pThis)
    {
	return objref( new int_object(pContext, N_INT(pThis) ));
    }

    objref enum_str(context* pContext, enumref pThis )
    {
	return pThis->str();
    }

    objref float_add(context* pContext, floatref a, objref b)
    {
	b = ::object::convert_to(b, builtins::flfloat::get_class() );
 
	// Check the result of the conversion
	if (!b)
	    throw eval_exception(cerror::unsupported_argument,"Can't add this type to a float");

	// Cast
	floatref pB = std::dynamic_pointer_cast<float_object>(b);

	const double result = a->internal_value() + pB->internal_value();
	objref pObject(new float_object(pContext,result));
    
	return pObject;
    }

    objref int_to_bool(context* pContext, intref pThis)
    {
	return boolref( new bool_object(pContext, N_INT(pThis)!=0) );

    }

    objref bool_to_int(context* pContext, boolref pThis)
    {
	intref pReturn = intref( new int_object(pContext,N_BOOL(pThis)?1:0) );
	return pReturn;
    }

    objref float_to_int(context* pContext, floatref pThis)
    {
	intref pReturn = intref( new int_object(pContext,std::round(N_FLOAT(pThis))) );
	return pReturn;
    }

    objref obj_convert(context* pContext, objref pThis, classref pTargetClass )
    {
	return ::object::convert_to( pThis, N_CLASS(pTargetClass) );
    }
    
}
