#ifndef OBJ_LAZY_H
#define OBJ_LAZY_H

#include <inc/references.h>
#include <interpreter/obj/object.h>

class lazy_object : public object
{
 public:
    lazy_object(context*,const astref& value=0,fclass& = *builtins::lazy::get_class());
    const astref& internal_value() const;
 protected:

    astref _value;
};

#endif
