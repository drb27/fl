#ifndef OBJ_VOID_H
#define OBJ_VOID_H

#include <interpreter/obj/object.h>

class void_object : public object
{
public:
    void_object(context* pContext,
		fclass& cls = *builtins::flvoid::get_class() );

    virtual void render( std::ostream& os, bool abbrev=true);    
    virtual bool operator==( const objref other) const;
};

#endif
