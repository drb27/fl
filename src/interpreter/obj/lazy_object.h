#ifndef OBJ_LAZY_H
#define OBJ_LAZY_H

#include <interpreter/obj/foundation.h>

class ast;

class lazy_object : public foundation_object<ast*>
{
public:
    lazy_object(context*,ast* value=0,fclass& = *builtins::lazy::get_class());
};

#endif
