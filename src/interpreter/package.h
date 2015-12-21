
#ifndef PACKAGE_H
#define PACKAGE_H

#include <string>
#include "base.h"
#include "named.h"
#include "nametable.h"
#include "class.h"

class package : public named, public static_base<package>
{

 public:
    package( const std::string&, const package&);
    package( const std::string&);

    virtual bool add_class( fclass& );

 private:
    nametable<fclass> classTable;
};

#endif
