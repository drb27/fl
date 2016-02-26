#ifndef OBJ_INTEGER_H
#define OBJ_INTEGER_H

#include <interpreter/obj/foundation.h>

class int_object : public foundation_object<int>
{
public:
    int_object(context*,int value=0,fclass& = *builtins::integer::get_class());
};

#endif
