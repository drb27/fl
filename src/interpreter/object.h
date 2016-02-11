#ifndef OBJECT_H
#define OBJECT_H

#include <memory>
#include <map>
#include <list>
#include <ostream>
#include <functional>
#include <deque>
#include <string>
#include <list>
#include <vector>
#include "class.h"
#include <interpreter/marshall.h>
#include <interpreter/context.h>
#include <interpreter/smartlist.h>
#include <parser/rawfn.h>
#include <interpreter/builtins.h>

class ast;
class typemgr;

class object
{

 public:

    object(context*, fclass&, std::vector<objref> params = {});
    object( const object&) =delete;
    virtual ~object();

    fclass& get_class() const { return _class; }
    context* get_context() const { return _context; }
    virtual bool has_attribute(const std::string&) const;
    virtual objref get_attribute(const std::string&);
    virtual void set_attribute(const std::string&,objref);
    virtual bool has_method(const std::string&) const;
    virtual void render( std::ostream& os, bool abbrev=true );
    virtual void dump( std::ostream& out = std::cout);
    virtual void optimise() {}
    virtual objref invoke( const std::string& mthdName, context* pCtx, std::vector<objref>& params);

    virtual bool operator==(const objref other) const; 
    virtual object& operator=(const objref other);

    static objref convert_to( objref pThis, fclass* pOther);

 protected:
    virtual void construct(context* pContext, std::vector<objref>&);
    std::map<std::string,objref> _attributes;

 private:
    context* _context;
    fclass& _class;
};

class int_object : public object
{
public:
    int_object(context*,int value,fclass& = *builtins::integer::get_class());
    virtual void render( std::ostream& os, bool abbrev=true );
    int internal_value() const { return _value; }

    virtual bool operator==(const objref other) const; 

protected:
    const int _value;
};

class float_object : public object
{
public:
    float_object(context*,double value,fclass& = *builtins::flfloat::get_class() );
    virtual void render( std::ostream& os, bool abbrev=true );
    double internal_value() const { return _value; }

    virtual bool operator==(const objref other) const; 

protected:
    const double _value;
};

class enum_object : public int_object
{
public:
    enum_object(context* pContext, int value, const std::string& name, fclass& cls) 
	: int_object(pContext,value,cls),_name(name) {}
    virtual void render( std::ostream& os, bool abbrev=true);
    virtual stringref str();
private:
    const std::string _name;
};


class string_object : public object
{
public:
    string_object(context*,const std::string& value, fclass& = *builtins::string::get_class() );
    string_object( const string_object& );
    virtual void render( std::ostream& os, bool abbrev=true );
    const std::string& internal_value() const { return _value; }

    virtual stringref operator[](intref index) const;
    virtual bool operator==(const objref other) const; 

    virtual stringref operator+(const stringref other) const;
    virtual void inplace_join(const stringref other);

protected:
    std::string _value;
};

class class_object : public object
{
public:
    class_object(context*,fclass* pCls,fclass& = *builtins::flclass::get_class() );
    virtual void render( std::ostream& os, bool abbrev=true);
    fclass* internal_value() const { return _value; }
    virtual bool has_attribute(const std::string&) const;
    virtual objref get_attribute(const std::string&);
    virtual std::map<std::string,objref> class_attributes() { return _value->class_attributes(); }
protected:
    fclass* const _value;
};

class bool_object : public object
{
public:
    bool_object(context*,bool b, fclass& = *builtins::boolean::get_class() );
    virtual void render( std::ostream& os, bool abbrev=true);
    bool internal_value() const { return _value; }

    virtual bool operator==(const objref other) const; 

protected:
    const bool _value;
};

class list_object : public object
{
public:
    list_object(context*,fclass& = *builtins::list::get_class() );
    list_object(context*,std::list<objref> startingList, fclass& = *builtins::list::get_class());
    list_object(context*,smartlist*,fclass& = *builtins::list::get_class() );
    list_object(context*,const list_object&);
    virtual void render( std::ostream& os, bool abbrev=true);
    objref first();
    void append(objref e);
    void append(listref);
    void prepend(objref e);
    objref pop();
    objref get_element(size_t index);
    int size() const;
    int chunks() const;
    listref tail(context*) const;
    virtual void optimise();
protected:
    std::shared_ptr<smartlist> _pList;
};

class void_object : public object
{
public:
    void_object(context* pContext,fclass& cls = *builtins::flvoid::get_class() ) 
	: object(pContext,cls) {}
    virtual void render( std::ostream& os, bool abbrev=true);
    
    virtual bool operator==( const objref other) const;
};

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
    std::string _name{"(anonymous)"};
    bool _is_anon;
};

#endif
