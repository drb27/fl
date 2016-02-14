#ifndef OBJ_FLOAT_H
#define OBJ_FLOAT_H

#include <interpreter/obj/object.h>

class float_object : public object
{
public:
    float_object(context*,double value,fclass& = *builtins::flfloat::get_class() );
    virtual void render( std::ostream& os, bool abbrev=true );
    double internal_value() const { return _value; }

    virtual bool operator==(const objref other) const; 

protected:
    const double _value;
};

#endif
