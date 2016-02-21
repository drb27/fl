#ifndef COLLECTION_H
#define COLLECTION_H

#include <string>
#include <map>
#include <inc/references.h>
#include <parser/symspec.h>

class collection
{

 public:
    collection();
    virtual ~collection();

    virtual void define_symbol( const symspec&, objref );
    virtual objref resolve_symbol(const symspec& ) const;
    virtual bool is_defined(const symspec& ) const;

    typedef std::map<std::string,objref>::iterator iterator;
    typedef std::map<std::string,objref>::const_iterator const_iterator;

    inline const_iterator begin(void) const { return _map.begin(); }
    inline const_iterator end(void) const { return _map.end(); }

 protected:

    virtual std::map<std::string,objref>::const_iterator find_impl( const symspec& target ) const;
    inline const std::map<std::string,objref>& map() const { return _map; }

 private:
    std::map<std::string,objref> _map;
};

class closure_collection : public collection
{

public:

    virtual void define_symbol( const symspec&, objref );
    
protected:
    
    virtual std::map<std::string,objref>::const_iterator find_impl( const symspec& target ) const;
};

#endif
