#ifndef OBJECT_H
#define OBJECT_H

#include <map>
#include <list>
#include <ostream>
#include "class.h"

class object
{

 public:

    object(const fclass&);
    virtual ~object();

    const fclass& get_class() const { return _class; }
    bool has_attribute(const std::string&) const;
    static const fclass& root_object_class();

    virtual void render( std::ostream& os ) const;

 protected:

 private:
    const fclass& _class;
    std::map<std::string,object*> _attributes;
    static const fclass _objectclass;
};

class ftinteger : public object
{
  public:
    ftinteger(int);
    virtual void render(std::ostream&) const;
  private:
    const int _value;
    static const fclass _intclass;
};

class ftlist : public object
{

  public:
    ftlist( const fclass& etype );
    void append( object* obj);

    virtual void render(std::ostream&) const;

  protected:

  private:
    const fclass& _etype;
    std::list<object*> _objects;

    static const fclass& class_factory( const fclass& etype );
    static std::map<const fclass*,const fclass*> _typemap;
};


#endif
