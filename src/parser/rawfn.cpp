#include <logger/logger.h>
#include <parser/rawfn.h>
#include <interpreter/context.h>
#include <parser/ast/fundef.h>

using std::vector;

rawfn::rawfn(ast* def, colref closure)
    : _closure(closure) 
{
    regenerate_function(def);
}

rawfn::rawfn(std::function<marshall_fn_t> f)
    : _closure(nullptr),_def(nullptr),_fn(f) 
{
}

objref rawfn::operator()(context* pCtx, vector<ast*>& args)
{
    return _fn(pCtx,args);
}

ast* rawfn::def()
{
    return _def;
}

bool rawfn::is_builtin() const
{
    return (!_def);
}

void rawfn::regenerate_function(ast* newDef)
{
    _def = newDef;
    fundef_node* localDef = dynamic_cast<fundef_node*>(newDef);
    colref pClosure = _closure;
    _fn = [localDef,pClosure](context* pContext, vector<ast*>& arglist)
	{
	    state_guard g(pContext);
	    wlog_entry();
	    wlog(level::debug,"Executing lambda for fl function call...");
	    wlog(level::debug,"Merging in the closure...");
	    g.new_collection(pClosure);
	    g.new_collection();
	    wlog_trace("Final context",pContext->trace());
	    wlog(level::debug,"Evaluating function definition...");
	    auto retVal = localDef->def()->evaluate(pContext);
	    wlog(level::debug,"About to return result from fl function call lambda...");
	    return retVal;
	};
}
