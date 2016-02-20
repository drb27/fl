#ifndef SYMSPEC_H
#define SYMSPEC_H

#include <string>
#include <list>

class symspec
{
 public:

    symspec( const std::string& n )
	: symspec(n,{}) {}


    symspec( const std::string& n, const std::list<std::string>& s)
	: _name(n),_pkg_spec(s) {}

    std::string rqn() const;

    inline const std::string& name() const { return _name; }
    inline const std::list<std::string>& pkg_spec() const { return _pkg_spec; }

    bool operator<(const symspec& other) const;

 private:

    const std::string _name;
    const std::list<std::string> _pkg_spec;
};


#endif
