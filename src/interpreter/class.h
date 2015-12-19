#ifndef CLASS_H
#define CLASS_H

#include <string>
#include "base.h"
#include "named.h"

class fclass : public named, public static_base<fclass>
{

 public:
    fclass(const std::string&, const fclass& base);
    fclass(const std::string& );
 
 protected:


 private:

};

#endif
