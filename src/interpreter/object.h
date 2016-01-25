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

class ast;
class typemgr;

class object
{

 public:

    object(context*, fclass&, std::vector<objref> params = {});
    object( const object&) =delete;
    virtual ~object();

    fclass& get_class() const { return _class; }
    bool has_attribute(const std::string&) const;
    objref get_attribute(const std::string&);
    virtual void set_attribute(const std::string&,objref);
    virtual void render( std::ostream& os ) const;
    virtual void dump( std::ostream& out = std::cout) const;

    virtual objref invoke( const std::string& mthdName, context* pCtx, std::vector<objref>& params);

    virtual bool operator==(const objref other) const; 
    virtual object& operator=(const objref other);

 protected:
    virtual void construct(context* pContext, std::vector<objref>&);
    std::map<std::string,objref> _attributes;

 private:
    fclass& _class;
    std::map<std::string,std::function<marshall_fn_t>> _methods;
};

class int_object : public object
{
public:
    int_object(context*,int value, fclass&,bool attr=true);
    virtual void render( std::ostream& os ) const;
    int internal_value() const { return _value; }

    virtual bool operator==(const objref other) const; 

protected:
    const int _value;
};

class string_object : public object
{
public:
    string_object(context*,const std::string& value, fclass&);
    virtual void render( std::ostream& os ) const;
    const std::string& internal_value() const { return _value; }

    virtual bool operator==(const objref other) const; 

protected:
    const std::string _value;
};

class class_object : public object
{
public:
    class_object(context*,fclass* pCls,fclass&);
    virtual void render( std::ostream& os) const;
    fclass* internal_value() const { return _value; }
protected:
    fclass* const _value;
};

class bool_object : public object
{
public:
    bool_object(context*,bool b, fclass&);
    virtual void render( std::ostream& os) const;
    bool internal_value() const { return _value; }

    virtual bool operator==(const objref other) const; 

protected:
    const bool _value;
};

class list_object : public object
{
public:
    list_object(context*,fclass&);
    list_object(context*,fclass&,std::list<objref> startingList);
    virtual void render( std::ostream& os) const;
    std::list<objref>& internal_value() { return _list; }
    objref first() const { return _list.front(); }
    void append(objref e) { _list.push_back(e); }
    void prepend(objref e) { _list.push_front(e); }
    listref tail(context*) const;
protected:
    std::list<objref> _list;
};

class void_object : public object
{
public:
    void_object(context* pContext,fclass& cls) : object(pContext,cls) {}
    virtual void render( std::ostream& os) const;
    
    virtual bool operator==( const objref other) const;
};

// A function which can only be evaluated when all of its arguments have been
// supplied. Partial arguments can be supplied. If this happens, *another* function
// object is returned which only needs the remaining arguments to be supplied. 
class fn_object : public object
{
public:

    typedef std::pair<std::string,objref> argpair_t;

    fn_object(context*, 
	      fclass&, 
	      std::function<marshall_fn_t> impl, 
	      std::deque<std::string> fullArgs,
	      collection&& appliedArgs );

    fn_object( context*, const fn_object&);
    virtual void render( std::ostream& os) const;
    
    virtual fnref partial_application( context*,const std::vector<argpair_t>& args ) const;
    virtual objref operator()(context*,std::vector<argpair_t>&);
    virtual objref operator()(context*,std::vector<objref>&);
    virtual const std::deque<std::string>& arglist() const;
    virtual void dump(std::ostream& out = std::cout ) const;

protected:

    std::function<marshall_fn_t> _fn;
    collection _applied_arguments;
    std::deque<std::string> _expected_args;
    std::deque<std::string> _full_args;
};

#endif
