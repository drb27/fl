#ifndef CLASS_H
#define CLASS_H

#include <functional>
#include <string>
#include <map>
#include <set>
#include <deque>
#include <list>
#include <parser/ast.h>
#include <interpreter/marshall.h>

class object;
class typemgr;

struct methodinfo
{
    std::string name;
    std::function<marshall_mthd_t> fn;
    bool sealed{false};
};

class typespec
{
  public:
  
    typespec(const std::string&);
    virtual ~typespec();

    const std::string& full_name() const;

    bool operator==( const typespec& other) const;
    int operator<( const typespec& other) const;
    int operator>( const typespec& other) const;
    

  protected:

  private:

    const std::string _name;
};


class conversion_tree_node;
typedef std::shared_ptr<conversion_tree_node> ctnoderef;

struct conversion_tree_node
{
    conversion_tree_node(ctnoderef p,fclass* c) : parent(p),cls(c) {}
    conversion_tree_node(fclass* c) : conversion_tree_node( ctnoderef(nullptr), c ) {}
    bool is_root() const { return !parent; }
    fclass* cls;
    ctnoderef parent;
    std::set<ctnoderef> subordinates;

    int length()
    {
	if (is_root())
	    return 1;
	else
	    return 1 + parent->length();
    }
};

class fclass
{

 public:
    fclass(const typespec&, fclass* pBase=nullptr, bool abstract=false,
	   bool builtin=false, bool allownew=true);
    std::string name() const;
    const typespec& get_spec() const;
    virtual void add_attribute(const std::string&,objref d);
    virtual void add_class_attribute(const std::string&,objref d);
    virtual const std::map<std::string,objref>& attributes() const;
    virtual const std::map<std::string,objref>& class_attributes();
    virtual void add_method(const methodinfo&);
    virtual bool has_method(const std::string& );
    virtual void add_class_method(const methodinfo&);
    virtual bool has_class_method(const std::string& );
    virtual std::list<std::string> methods() const;
    virtual void all_methods( std::set<std::string>& ) const;

    virtual const methodinfo& lookup_method(const std::string& name) const;
    virtual const methodinfo& lookup_class_method(const std::string& name) const;
    virtual fclass* base(void) const { return _base; }
    virtual std::deque<fclass*> hierarchy();
    virtual bool is_in_hierarchy( const fclass& other);
    virtual const methodinfo& instantiator() { return lookup_method(".ctor"); }
    virtual bool can_convert_to(fclass* pOther);
    virtual bool build_conversion_tree(fclass* pGoal,std::set<ctnoderef>& solutionSet);

    bool is_root() const { return _base==nullptr; }
    bool is_abstract() const { return _is_abstract; }
    bool is_builtin() const { return _is_builtin; }
    bool allow_new() const { return _allow_new; }

    static typemgr* types;
 protected:

    ctnoderef get_all_conversions(std::set<fclass*>& inclusionSet, 
				  std::set<ctnoderef>& solutionSet,
				  fclass* pGoal);
    void get_direct_conversions(std::set<fclass*>&);
 private:

    fclass* _base;
    const bool _is_abstract;
    const bool _is_builtin;
    const bool _allow_new;
    const typespec _ts;
    std::map<std::string,objref> _attributes;
    std::map<std::string,methodinfo> _methods;

    std::map<std::string,methodinfo> _class_methods;
    std::map<std::string,objref> _class_attributes;
};

#endif
