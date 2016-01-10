#ifndef CALLABLE_H
#define CALLABLE_H

#include <tuple>
#include <vector>
#include <memory>

#include <interpreter/context.h>
#include <interpreter/object.h>
#include <parser/ast.h>

class object;
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
    eval_each(std::array<ast*,sizeof...(P)>&, std::tuple<P...>&) {}

    template<std::size_t I=0, typename... P>
    inline typename std::enable_if<I < sizeof...(P), void>::type
    eval_each(std::array<ast*,sizeof...(P)>& p,std::tuple<P...>& t)
    {
	auto result = std::get<I>(p)->evaluate(nullptr);
	std::get<I>(t) = std::dynamic_pointer_cast<typename std::tuple_element<I,std::tuple<P...>>::type::element_type>(result);
	eval_each<I+1,P...>(p,t);
    }

    template<std::size_t I=0, typename... P>
    inline typename std::enable_if<I==sizeof...(P),void>::type
    v_eval_each(std::vector<ast*>&, std::tuple<P...>&) {}

    template<std::size_t I=0, typename... P>
    inline typename std::enable_if<I < sizeof...(P), void>::type
    v_eval_each(std::vector<ast*>& p,std::tuple<P...>& t)
    {
	auto result = p[I-1]->evaluate(nullptr);
	std::get<I>(t) = std::dynamic_pointer_cast<typename std::tuple_element<I,std::tuple<P...>>::type::element_type>(result);
	v_eval_each<I+1,P...>(p,t);
    }

    template<typename T> struct ArgTuple;

    template<typename R, typename ... Args>
    struct ArgTuple<R(&)(Args...)>
    {
	typedef std::tuple<Args...> type;
    };

    typedef objref (marshall_fn_t)(std::vector<ast*>&);

    template<typename F>
    std::function<marshall_fn_t> make_marshall( F&& f, context* pContext)
    {
	auto fp = &f;
	return [fp,pContext](std::vector<ast*>& p)
	{
	    typename ArgTuple<F>::type evaled_params;
	    std::get<0>(evaled_params) = pContext;
	    v_eval_each<1>(p,evaled_params);
	    return apply(*fp,evaled_params);
	};
    }

}

class i_callable
{
public:
    virtual objref operator()(void)=0;
};

template<typename R,typename... P>
class typed_method : public i_callable
{
public:
    typed_method() {}
    virtual void set_params( const std::array<ast*,sizeof...(P)>& p)
    {
	_params = p;
    }

protected:
    std::array<ast*,sizeof...(P)> _params;
};

template<typename R, typename... P>
class internal_typed_method : public typed_method<R,P...>
{
    
public:
    typedef R (fn_t)(P...);
    internal_typed_method(context* pContext,fn_t* pFn) : _function(pFn),_context(pContext) {}

    virtual objref operator()(void)
    {
	// Evaluate all of the parameters
	std::tuple<P...> evaled_params;
	eval_each<1,P...>(this->_params,evaled_params);
	std::get<0>(evaled_params) = _context;

	// Dispatch the call
	return apply(_function,evaled_params);
    }

private:
    std::function<fn_t> _function;
    context* _context;
};

#endif
