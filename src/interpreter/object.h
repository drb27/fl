#ifndef OBJECT_H
#define OBJECT_H

#include <map>
#include <list>
#include <ostream>
#include "class.h"

class ast;
class typemgr;
class icallable;

class object
{

 public:

    object(const fclass&);
    virtual ~object();

    const fclass& get_class() const { return _class; }
    bool has_attribute(const std::string&) const;
    virtual void render( std::ostream& os ) const;

 protected:

 private:
    const fclass& _class;
    std::map<std::string,object*> _attributes;
    std::map<std::string,icallable*> _methods;

};

class int_object : public object
{
public:
    int_object(int value, const fclass&);
    virtual void render( std::ostream& os ) const;
    int internal_value() const { return _value; }
protected:
    const int _value;
};

#endif
