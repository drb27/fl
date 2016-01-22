#ifndef CLASS_H
#define CLASS_H

#include <functional>
#include <string>
#include <map>
#include <list>
#include "named.h"
#include <parser/ast.h>
#include <interpreter/marshall.h>

class object;

class typespec
{
  public:
  
    typespec(const std::string&, const std::list<typespec>&);
    virtual ~typespec();

    int param_count() const;
    const std::list<typespec>& params() const;
    const std::string& template_name() const;
    const std::string& full_name() const;

    bool operator==( const typespec& other) const;
    int operator<( const typespec& other) const;
    int operator>( const typespec& other) const;
    

  protected:
  
  private:

    std::string calc_full_name();
    void validate() const;

    const std::string _name;
    std::string _full_name;
    std::list<typespec> _params;
    
};

class fclass
{

 public:
    fclass(const typespec&, fclass* pBase, bool abstract=false);
    std::string name() const;
    const typespec& get_spec() const;
    virtual void add_attribute(const std::string&,fclass*,object* d=nullptr);
    virtual void add_method(const std::string&,std::function<marshall_mthd_t>);
    virtual std::list<std::string> methods() const;
    virtual bool is_abstract() const { return _is_abstract; }

    virtual std::function<marshall_mthd_t> lookup_method(const std::string& name) const;
    virtual fclass* base(void) const { return _base; }
    virtual bool is_root() const { return _base==nullptr; }
 protected:

 private:

    fclass* _base;
    const bool _is_abstract;
    const typespec _ts;
    std::map<std::string,fclass*> _attributes;
    std::map<std::string,std::function<marshall_mthd_t>> _methods;
};

#endif
