#ifndef OBJECT_H
#define OBJECT_H

#include <ostream>
#include <map>
#include <string>
#include <vector>

#include <interpreter/class.h>
#include <interpreter/marshall.h>
#include <interpreter/builtins.h>

class context;

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
    virtual void create_attribute(const std::string&);
    virtual bool has_method(const std::string&) const;
    virtual void render( std::ostream& os, bool abbrev=true );
    virtual void dump( std::ostream& out = std::cout);
    virtual void optimise() {}
    virtual objref invoke( const std::string& mthdName, 
			   context* pCtx, 
			   std::vector<objref>& params);
    
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

#endif
