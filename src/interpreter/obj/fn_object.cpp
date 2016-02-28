#include <algorithm>
#include <interpreter/obj/fn_object.h>
#include <logger/logger.h>
#include <interpreter/eval_exception.h>
#include <interpreter/context.h>
#include <parser/ast/symbol.h>

using std::string;
using std::vector;
using std::endl;
using std::pair;

fn_object::fn_object(context* pContext,
		     rawfn impl, 
		     hinted_args_t fullArgs,
		     hinted_args_t origArgs,
		     collection&& appliedArgs,
		     fclass& cls)
    : _full_args(fullArgs), _orig_args(origArgs), _expected_args(fullArgs), 
      object(pContext,cls), _fn(impl)
{
    wlog_entry();
    _is_anon=true;
    _applied_arguments=std::move(appliedArgs);
}

void fn_object::set_name(const string& fname)
{
    if (_is_anon)
    {
	_name = fname;
	_is_anon=false;
    }
}

const string& fn_object::name() const
{
    return _name;
}

void fn_object::render(std::ostream& os, bool abbrev)
{
    object::render(os,abbrev);
}

void fn_object::dump( std::ostream& out)
{
    object::dump(out);

    out << "Expected Args: ";
    for ( auto arg : _expected_args )
    {
	out << arg.first << " ";
    }
    out << endl;

    out << "Full Args: ";
    for ( auto arg : _full_args )
    {
	out << arg.first << " ";
    }
    out << endl;

}

fnref fn_object::partial_application(context* pContext,const vector<argpair_t>& args) const
{
    wlog_entry();
    hinted_args_t remainingArgs(_full_args);
    collection appliedArgs = _applied_arguments;

    // For each partial application, add to the context and remove from
    // the remaining args
    for ( auto arg : args )
    {
	appliedArgs.define_symbol(arg.first,arg.second);
	remainingArgs.erase(std::find_if(remainingArgs.begin(),remainingArgs.end(),
					 [&arg](pair<string,ast*>& e){ return e.first==arg.first;}));
    }
    
    auto result = fnref( new fn_object(pContext,_fn, remainingArgs, _orig_args,
				       std::move(appliedArgs), get_class() ));
    result->_is_anon = _is_anon;
    result->_name = _name;
    return result;

}

rawfn& fn_object::raw()
{
    return _fn;
}

auto fn_object::arglist() const -> const hinted_args_t&
{
    return _full_args;
}

bool fn_object::is_anonymous() const
{
    return _is_anon;
}

bool fn_object::is_tail_recursive()
{
    // TODO
    return _fn.def()->calls_and_returns(_name);
}

void fn_object::optimise_tail_recursion(context*)
{

}

objref fn_object::operator()(context* pContext, vector<objref>& args)
{
    vector<argpair_t> argpairs;

    if ( args.size()>_full_args.size() )
	throw eval_exception(cerror::too_many_arguments, "Too many argument supplied to function call");

    if ( args.size()==_full_args.size() )
    {
	// Zip 'em all up
	int index=0;
	for ( auto a : _full_args )
	{
	    argpairs.push_back(argpair_t(a.first,args[index++]));
	}
    }
    else
    {
	// Just the expected args
	int index=0;
	for ( auto a : args )
	{
	    argpairs.push_back(argpair_t(_expected_args[index++].first,a));
	}
    }

    return (*this)(pContext,argpairs);
}

objref fn_object::operator()(context* pContext, vector<argpair_t>& args)
{
    wlog_entry();

    if ( args.size()>_full_args.size() )
	throw eval_exception(cerror::too_many_arguments, "Too many argument supplied to function call");

    // Is this a full or partial application?
    if (args.size()==_full_args.size() )
    {
	state_guard g(pContext);
	pContext->new_collection();
	vector<ast*> params;

	// TODO: To support built-in functions, we need to add
	// previously-applied arguments to the params vector. Because they are
	// stored in a map, we don't know which order. So alongside _full_args,
	// we need to store _original_args, to preserve this ordering. 

	// Make a copy of _orig_args
	auto prev_args = _orig_args;

	// Remove arguments that are in _full_args
	for ( auto arg : _full_args )
	{
	    prev_args.erase(std::find_if(prev_args.begin(),prev_args.end(),
					 [&arg](pair<string,ast*>& e)
					 { 
					     return e.first==arg.first;
					 } ));
	}

	// prev_args now is an ordered sequence of previously-applied
	// arguments. Add to params in order, taking the value from
	// _applied_arguments.
	for ( auto arg : prev_args)
	{
	    params.push_back( new symbol_node(arg.first));
	}


	// Now push the new arguments onto params
	for ( auto arg : args )
	{
	    params.push_back( new symbol_node(arg.first) );
	    pContext->assign(arg.first,arg.second);
	}

	for ( auto aa : _applied_arguments )
	{
	    pContext->assign(aa.first,aa.second);
	}
	
	// Apply the accrued arguments to the marshall function
	auto result = _fn(pContext,params);
	return result;
    }
    else
	return partial_application(pContext,args);
}

fn_object::fn_object(context* pContext, const fn_object& other)
    : object(pContext,other.get_class()), _fn(other._fn), 
      _applied_arguments(other._applied_arguments), _orig_args(other._orig_args), 
      _expected_args(other._expected_args), _full_args(other._full_args),
      _name(other._name)
{
    wlog_entry();
    _is_anon=other._is_anon;
}
