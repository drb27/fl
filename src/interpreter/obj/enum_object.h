#ifndef OBJ_ENUM_H
#define OBJ_ENUM_H

#include <interpreter/obj/int_object.h>

class enum_object : public int_object
{
public:

    enum_object(context* pContext, 
		int value, const 
		std::string& name, fclass& cls);

    virtual void render( std::ostream& os, bool abbrev=true);
    virtual stringref str();

private:
    const std::string _name;
};

#endif
