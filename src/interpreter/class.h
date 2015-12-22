#ifndef CLASS_H
#define CLASS_H

#include <string>
#include <map>
#include <list>
#include "base.h"
#include "named.h"

class typespec
{
  public:
  
    typespec(const std::string&);
    virtual ~typespec();

    int param_count() const;
    const std::list<typespec>& params() const;
    std::list<typespec>& params();
    const std::string& template_name() const;
    std::string full_name() const;
    void push_param( typespec& );

    bool operator==( const typespec& other) const;

  protected:
  
  private:

    const std::string _name;
    std::list<typespec> _params;
};

class fclass : public static_base<fclass>
{

 public:
    fclass(const typespec&, const fclass& base);
    fclass(const typespec& );
    std::string name() const;
 protected:

 private:

    const typespec _ts;
    std::map<std::string,fclass*> _attributes;
};

#endif
