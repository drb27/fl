#ifndef CLASS_H
#define CLASS_H

#include <string>
#include <map>
#include <list>
#include "base.h"
#include "named.h"

class icallable;
class object;

class typespec
{
  public:
  
    typespec(const std::string&, const std::list<typespec>&);
    virtual ~typespec();

    int param_count() const;
    const std::list<typespec>& params() const;
    const std::string& template_name() const;
    std::string full_name() const;

    bool operator==( const typespec& other) const;
    int operator<( const typespec& other) const;
    int operator>( const typespec& other) const;

  protected:
  
  private:

    void validate() const;

    const std::string _name;
    std::list<typespec> _params;
};

class fclass : public static_base<fclass>
{

 public:
    fclass(const typespec&, const fclass& base);
    fclass(const typespec& );
    std::string name() const;
    const typespec& get_spec() const;
    virtual void add_attribute(const std::string&,fclass*,object* d=nullptr);
 protected:

 private:

    const typespec _ts;
    std::map<std::string,fclass*> _attributes;
    std::map<std::string,icallable*> _methods;
};

#endif
