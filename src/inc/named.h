#ifndef NAMED_H
#define NAMED_H

#include <string>

class named
{

 public:
    const std::string& name(void) const
    {
	return _name;
    }

 protected:
    named( const std::string& name) : _name(name) {}
    

 private:
    const std::string _name;
};

#endif
