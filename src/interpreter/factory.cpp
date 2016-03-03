#include <interpreter/factory.h>
#include <parser/ast/literal.h>
#include <parser/ast/list.h>

#include <interpreter/obj/list_object.h>
#include <interpreter/obj/int_object.h>
#include <interpreter/obj/lazy_object.h>

#include <interpreter/context.h>

/**
 * @class factory
 * The factory class implements the nuts-and-bolts of creating and initializing fl
 * objects.
 *
 * How object construction works:
 * Note that an fl class can be either built-in or native fl. Each fl object is
 * represented by a c++ object behind the scenes. Note also that a non-builtin class can
 * inherit from a builtin class. 
 *
 * There are two sets of constructors that get executed when an fl object is created. The
 * first is the chain of c++ constructors. These set up the c++ object, including creating
 * any attributes that are defined on the fl object it represents. This set is executed
 * first. 
 *
 * The second set of constructors is the set of fl constructors that are defined on the fl
 * class hierarchy. An fl constructor has two components - a method body, and a list of
 * parameters, known as the chain parameters, used to invoke the constructor of the base
 * class. Note that the fl constructors are executed from the super-most class down to the
 * class of the object being created. fl constructors can be c++ functions or fl functions
 * (the abstraction being done using the make_marshall_mthd() mechanism). 
 */

using std::vector;

factory* factory::_factory_singleton=nullptr;

factory::factory()
{
}

factory& factory::get()
{
    if (!_factory_singleton)
	_factory_singleton = new factory();

    return (*_factory_singleton);
}

objref factory::spawn_object(context* pContext,fclass* pCls)
{
    auto spawnBase = pCls;

    while ( _ctor_map.find(spawnBase)==_ctor_map.end() )
	spawnBase = spawnBase->base();

    return objref( _ctor_map[spawnBase](pContext,pCls) );
}

void factory::add_spawner(fclass* pCls, std::function<objref(context*,fclass*)> pFn )
{
    _ctor_map[pCls] = pFn;
}

objref factory::bootstrap_integer(context* pContext, int x)
{
    return objref(new int_object(pContext,x));
}

objref factory::make_object( context* ctx, fclass* cls, vector<ast*>& a )
{
    objref pNewObj = spawn_object(ctx,cls);
    call_ctor_chain(ctx,cls,pNewObj,a);
    return pNewObj;
}

objref factory::make_object( context*, fclass*, vector<objref>& )
{
    // TODO: #43 implement make_object
    return objref(nullptr);
}

objref factory::make_object( context* ctx, fclass* cls, listref l )
{
    vector<ast*> a_params = list_to_ast(l);
    auto r = make_object(ctx,cls,a_params);
    for ( auto p : a_params)
	delete p;
    return r;
}

vector<ast*> factory::list_to_ast(listref l )
{
    const int item_count = l->size();
    vector<ast*> a_params(item_count);
    for ( int index=0; index < item_count; index++ )
    {
	a_params[index] = new literal_node( l->get_element(index) );
    }

    return a_params;
}

void factory::call_ctor_chain(context* pContext, fclass* pCls, objref pThis, vector<ast*>& params )
{
    // Get constructor info for this class
    ctorinfo& c = pCls->ctor();
    const bool isRoot = pCls->is_root();

    // If there is no constructor defined, skip immediately to the base class
    if (!c.defined)
    {
	if (!isRoot)
	    call_ctor_chain(pContext,pCls->base(),pThis,params);
	return;
    }

    // If this class is not root, call the ctor chain of the base class
    if (!isRoot)
    {
	vector<ast*> chained_params;
	// Evaluate the chain params in the current context, with params applied
	{
	    state_guard g(pContext);
	    pContext->new_collection();

	    auto paramIdx = c.args.begin();
	    pContext->assign( *paramIdx, pThis );
	    paramIdx++;
	    for ( auto p : params )
	    {
		pContext->assign( *paramIdx, p->evaluate(pContext) );
		paramIdx++;
	    }
	    
	    if (!c.chain_params )
	    {
		list_node* pEmptyList = new list_node();
		c.chain_params = lazyref( new lazy_object( pContext,astref(pEmptyList) ) );
	    }
	    auto evaluatedChain = c.chain_params->internal_value()->evaluate(pContext);
	    listref evaluatedChainList = object::cast_or_abort<list_object>(evaluatedChain);
	    chained_params = list_to_ast(evaluatedChainList);

	}
	call_ctor_chain(pContext,pCls->base(),pThis,chained_params);
	for ( auto p : chained_params)
	    delete p;
    }
    
    if (c.defined)
    {
	// Now execute the constructor of this class
	call_constructor(pContext,c,pThis,params);
    }
}

void factory::call_ctor_chain(context* pContext, fclass* pCls, objref pThis, listref params )
{
    auto a_params = list_to_ast(params);

    call_ctor_chain(pContext,pCls,pThis,a_params);

    for ( auto p : a_params )
	delete p;
}

void factory::call_constructor( context* pContext, ctorinfo c, objref pThis, listref params )
{
    auto a_params = list_to_ast(params);
    call_constructor(pContext,c,pThis,a_params);

    for ( auto p : a_params )
	delete p;
}

void factory::call_constructor( context* pContext, ctorinfo c, objref pThis, vector<ast*>& params )
{
    if (c.defined)
    {
	auto pc = params;
	pc.insert( pc.begin(), nullptr );
	pc.insert( pc.begin(), nullptr );
	c.fn(pContext,pThis,pc);
    }
}
