#ifndef CALLABLE_H
#define CALLABLE_H

#include <tuple>

class object;
class ast;

namespace
{
    template<typename R,typename F, typename T, bool Done, int Total, int... N>
    struct call_impl
    {
	static R call( F f, T&& t)
	{
	    return call_impl<R,F,T,Total==1+sizeof...(N),Total, N..., sizeof...(N)>::call(f,std::forward<T>(t));
	}
    };

    template<typename R,typename F, typename T,int Total, int... N>
    struct call_impl<R,F,T,true,Total,N...>
    {
	static R call( F f, T&& t)
	{
	    return f(std::get<N>(std::forward<T>(t))...);
	}
    };

    template<std::size_t I=0, typename... P>
    inline typename std::enable_if<I==sizeof...(P),void>::type
    eval_each(std::array<ast*,sizeof...(P)>&, std::tuple<P...>&) {}

    template<std::size_t I=0, typename... P>
    inline typename std::enable_if<I < sizeof...(P), void>::type
    eval_each(std::array<ast*,sizeof...(P)>& p,std::tuple<P...>& t)
    {
	// OLD: std::get<I>(t) = dynamic_cast<typename std::tuple_element<I,std::tuple<P...>>::type>(std::get<I>(p)->evaluate(nullptr));
	auto result = std::get<I>(p)->evaluate(nullptr);
	std::get<I>(t) = std::dynamic_pointer_cast<typename std::tuple_element<I,std::tuple<P...>>::type::element_type>(result);
	eval_each<I+1,P...>(p,t);
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
    internal_typed_method(fn_t* pFn) : _function(pFn) {}

    virtual objref operator()(void)
    {
	// Evaluate all of the parameters
	std::tuple<P...> evaled_params;
	eval_each<0,P...>(this->_params,evaled_params);
	
	// Dispatch the call
	typedef typename std::decay<std::tuple<P...> >::type ttype;
	return call_impl<R,std::function<R(P...)>,std::tuple<P...>,0==std::tuple_size<ttype>::value,
			 std::tuple_size<ttype>::value>::call(_function,std::forward<std::tuple<P...> >(evaled_params));
    }

private:
    std::function<fn_t> _function;
};

#endif
