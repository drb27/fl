
#ifndef PACKAGE_H
#define PACKAGE_H

#include <string>
#include "base.h"
#include "named.h"

class package;

class package : public named,static_base<package>
{

 public:

    package( const std::string&, const package&);

};

#endif
