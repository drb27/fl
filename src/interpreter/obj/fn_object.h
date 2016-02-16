#ifndef OBJ_FN_H
#define OBJ_FN_H

#include <interpreter/obj/object.h>
#include <parser/rawfn.h>

/**
 * A function which can only be evaluated when all of its arguments have been
 * supplied. Partial arguments can be supplied. If this happens, *another* function
 * object is returned which only needs the remaining arguments to be supplied.
 * @note the fn_object can represet an fl function, or a builtin written in C++.  
 **/
class fn_object : public object
{
public:

    typedef std::pair<std::string,objref> argpair_t;
    typedef std::deque<std::pair<std::string,ast*>> hinted_args_t;

    fn_object(context*, 
	      rawfn impl, 
	      hinted_args_t fullArgs,
	      hinted_args_t origArgs,
	      collection&& appliedArgs,
	      fclass& = *builtins::function::get_class() );

    fn_object( context*, const fn_object&);
    virtual void render( std::ostream& os, bool abbrev=true);
    
    virtual fnref partial_application( context*,const std::vector<argpair_t>& args ) const;
    virtual objref operator()(context*,std::vector<argpair_t>&);
    virtual objref operator()(context*,std::vector<objref>&);
    virtual const hinted_args_t& arglist() const;
    virtual void dump(std::ostream& out = std::cout );
    virtual bool is_tail_recursive();
    virtual bool is_anonymous() const;

    virtual void set_name(const std::string& fname);
    virtual const std::string& name() const;
    virtual rawfn& raw();
    virtual void optimise_tail_recursion(context*);

protected:

    rawfn _fn;
    collection _applied_arguments;
    hinted_args_t _expected_args;
    hinted_args_t _full_args;
    hinted_args_t _orig_args;
    std::string _name{"(anonymous)"};
    bool _is_anon;
};

#endif
