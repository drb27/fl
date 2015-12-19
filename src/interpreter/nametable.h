#ifndef NAMETABLE_H
#define NAMETABLE_H

#include <map>
#include <string>
#include "named.h"

template<class T>
class nametable 
{
 public:
    typedef std::map<std::string,T*> map_t;
 
    bool contains(const std::string& n) const 
    { 
	return _map.find(n)!=_map.end();
    }

    bool contains(const T* e) const 
    { 
	return _map.find(e->name())!=_map.end();
    }
    
    T* get(const std::string& n) const
    { 
	return _map[n];
    }

    T* set(T* e, bool allowReplace=false)
    {
	auto retVal = _map.find(e->name());

	if ( allowReplace || retVal!=_map.end() )
	    _map[e->name()] = e;

	return (*retVal).second;
    }
    
protected:
    
private:
    map_t _map;
};

#endif
