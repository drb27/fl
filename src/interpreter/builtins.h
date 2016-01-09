#ifndef BUILTINS_H
#define BUILTINS_H

#include <memory>

class fclass;

namespace builtins
{

class object
{
public:
    static std::unique_ptr<fclass> build_class();
};

class integer
{
public:
    static std::unique_ptr<fclass> build_class();
};

class string
{

};

}

#endif
