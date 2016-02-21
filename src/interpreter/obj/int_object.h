#ifndef OBJ_INTEGER_H
#define OBJ_INTEGER_H

#include <interpreter/obj/object.h>

class int_object : public object
{
public:
    int_object(context*,int value,fclass& = *builtins::integer::get_class());
    virtual void render( std::ostream& os, bool abbrev=true );
    int internal_value() const { return _value; }

    virtual bool operator==(const objref other) const; 

protected:
    const int _value;
};

#endif
