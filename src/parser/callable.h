#ifndef CALLABLE_H
#define CALLABLE_H

#include <tuple>
#include <vector>
#include <memory>
#include <interpreter/object.h>
#include <interpreter/eval_exception.h>

class context;
class ast;

namespace
{
    template<typename F,typename T, size_t... I>
    decltype(auto) apply_impl(F&& f, T&& t, std::index_sequence<I...>)
    {
	return std::forward<F>(f)(std::get<I>(std::forward<T>(t))...);
    }

    template<typename F, typename T>
    decltype(auto) apply(F&& f, T&& t)
    {
	using Indices = std::make_index_sequence<std::tuple_size<std::decay_t<T>>::value>;
	return apply_impl(std::forward<F>(f), std::forward<T>(t), Indices{} );
    }

    template<std::size_t I=0, typename... P>
    inline typename std::enable_if<I==sizeof...(P),void>::type
    v_eval_each(context*,std::vector<ast*>&, std::tuple<P...>&) {}

    template<std::size_t I=0, typename... P>
    inline typename std::enable_if<I < sizeof...(P), void>::type
    v_eval_each(context* c,std::vector<ast*>& p,std::tuple<P...>& t)
    {
	auto result = p[I]->evaluate(c);
	std::get<I>(t) = std::dynamic_pointer_cast<typename std::tuple_element<I,std::tuple<P...>>::type::element_type>(result);
	v_eval_each<I+1,P...>(c,p,t);
    }

    template<typename T> struct ArgTuple;

    template<typename R, typename ... Args>
    struct ArgTuple<R(Args...)>
    {
	typedef std::tuple<Args...> type;
    };

    template<typename F>
    std::function<marshall_fn_t> make_marshall( F* f)
    {
	return [f](context* pContext, std::vector<ast*>& p)
	{
	    typename ArgTuple<F>::type evaled_params;
	    std::get<0>(evaled_params) = pContext;

	    // Experimental: prepare args with one additional element
	    std::vector<ast*> myArgs(p);
	    myArgs.insert( myArgs.begin(),nullptr);
	    v_eval_each<1>(pContext,myArgs,evaled_params);

	    // At this point, we have all of the evaluated params in evaled_params[1]..[n].
	    // The context is in evaled_params[0]. 
	    // The actual function arguments are in myArgs[1]..[n].

	    for ( int paramIndex=1; paramIndex<myArgs.size(); paramIndex++ )
	    {
		// Process type hints ...
		ast* pTypeHintNode = myArgs[paramIndex];
		if (pTypeHintNode)
		{
		    typespec tsc("class",{});
		    objref evaledTypeHint = pTypeHintNode->evaluate(pContext);
		    if ( &(evaledTypeHint->get_class())!=&(pContext->types()->lookup(tsc)))
		    {
			throw eval_exception(cerror::not_a_class,"Type hint does not evaluate to a class");
		    }

		    // TODO: Process the type hint!
		}
	    }

	    return apply(*f,evaled_params);
	};
    }

    template<typename F>
    std::function<marshall_mthd_t> make_marshall_mthd( F* f)
    {
	return [f](context* pContext, objref pThis, std::vector<ast*>& p)
	{
	    typename ArgTuple<F>::type evaled_params;
	    std::get<0>(evaled_params) = pContext;
	    std::get<1>(evaled_params) = std::dynamic_pointer_cast<typename std::tuple_element<1,typename ArgTuple<F>::type>::type::element_type>(pThis);
	    v_eval_each<2>(pContext,p,evaled_params);
	    return apply(*f,evaled_params);
	};
    }
}


#endif
