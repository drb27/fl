#ifndef OBJECT_H
#define OBJECT_H

#include <memory>
#include <map>
#include <list>
#include <ostream>
#include <functional>
#include <list>
#include "class.h"
#include <interpreter/marshall.h>

class ast;
class typemgr;

class object
{

 public:

    object(fclass&);
    virtual ~object();

    fclass& get_class() const { return _class; }
    bool has_attribute(const std::string&) const;
    virtual void render( std::ostream& os ) const;

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
protected:
    const int _value;
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
    virtual void render( std::ostream& os) const;
    std::list<objref>& internal_value() { return _list; }
protected:
    std::list<objref> _list;
};

class null_object : public object
{
public:
    null_object(fclass& cls) : object(cls) {}
    virtual void render( std::ostream& os) const;
};

#endif
