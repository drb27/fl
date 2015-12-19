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
 
    bool contains(const T* e) const 
    { 
	return contains(e->name());
    }

    bool contains(const std::string n) const 
    { 
	return _map.find(n)!=_map.end();
    }
    
    T* get(const std::string& n)
    { 
	return _map[n];
    }

    T* set(T* e, bool allowReplace=false)
    {
	auto retIter = _map.find(e->name());
	auto retObj = (*retIter).second;

	if ( allowReplace || (retIter==_map.end()) )
	    _map[e->name()] = e;

	if ( retIter!=_map.end())
	    return retObj;
	else
	    return nullptr;
    }

    int size(void) const
    {
	return _map.size();
    }
    
protected:
    
private:
    map_t _map;
};

#endif
