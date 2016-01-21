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
#include "class.h"
#include <interpreter/marshall.h>
#include <interpreter/context.h>

class ast;
class typemgr;

class object
{

 public:

    object(fclass&);
    object( const object&) =delete;
    virtual ~object();

    fclass& get_class() const { return _class; }
    bool has_attribute(const std::string&) const;
    virtual void render( std::ostream& os ) const;
    virtual bool equate( objref other ) const { return false; }
    virtual void dump( std::ostream& out = std::cout) const;

 protected:

 private:
    fclass& _class;
    std::map<std::string,object*> _attributes;
    std::map<std::string,std::function<marshall_fn_t>> _methods;

};

class int_object : public object
{
public:
    int_object(int value, fclass&);
    virtual void render( std::ostream& os ) const;
    int internal_value() const { return _value; }
    virtual bool equate( objref other ) const;
protected:
    const int _value;
};

class class_object : public object
{
public:
    class_object(fclass* pCls,fclass&);
    virtual void render( std::ostream& os) const;
    fclass* internal_value() const { return _value; }
protected:
    fclass* const _value;
};

class bool_object : public object
{
public:
    bool_object(bool b, fclass&);
    virtual void render( std::ostream& os) const;
    bool internal_value() const { return _value; }
protected:
    const bool _value;
};

class list_object : public object
{
public:
    list_object(fclass&);
    list_object(fclass&,std::list<objref> startingList);
    virtual void render( std::ostream& os) const;
    std::list<objref>& internal_value() { return _list; }
    objref first() const { return _list.front(); }
    listref append(objref e) { _list.push_back(e); }
    listref tail() const;
protected:
    std::list<objref> _list;
};

class null_object : public object
{
public:
    null_object(fclass& cls) : object(cls) {}
    virtual void render( std::ostream& os) const;
};

// A function which can only be evaluated when all of its arguments have been
// supplied. Partial arguments can be supplied. If this happens, *another* function
// object is returned which only needs the remaining arguments to be supplied. 
class fn_object : public object
{
public:

    typedef std::pair<std::string,objref> argpair_t;

    fn_object(fclass&, std::function<marshall_fn_t> impl, std::deque<std::string> args);
    fn_object( const fn_object&);
    virtual void render( std::ostream& os) const;
    
    virtual fnref partial_application( const std::vector<argpair_t>& args ) const;
    virtual objref operator()(context*,std::vector<argpair_t>&);
    virtual objref operator()(context*,std::vector<objref>&);
    virtual const std::deque<std::string>& arglist() const;
    virtual void dump(std::ostream& out = std::cout ) const;

protected:
    virtual void apply_argument( objref arg);
    virtual void apply_argument( const std::string& name, objref arg);

    std::function<marshall_fn_t> _fn;
    context _applied_arguments;
    std::deque<std::string> _expected_args;
    std::deque<std::string> _full_args;
};

#endif
