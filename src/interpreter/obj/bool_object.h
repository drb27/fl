#ifndef OBJ_BOOL_H
#define OBJ_BOOL_H

#include <interpreter/obj/object.h>

class bool_object : public object
{
public:

    bool_object(context*,
		bool b, 
		fclass& = *builtins::boolean::get_class() );

    virtual void render( std::ostream& os, bool abbrev=true);
    bool internal_value() const { return _value; }

    virtual bool operator==(const objref other) const; 

protected:
    const bool _value;

};

#endif
