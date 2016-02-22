#ifndef OBJ_CLASS_H
#define OBJ_CLASS_H

#include <interpreter/obj/object.h>

class class_object : public object
{
public:

    class_object(context*,
		 fclass* pCls,
		 fclass& = *builtins::flclass::get_class() );

    virtual void render( std::ostream& os, bool abbrev=true);
    virtual bool has_attribute(const std::string&) const;
    virtual objref get_attribute(const std::string&);
    virtual bool selector_match( const objref other) const;    
    std::map<std::string,objref> class_attributes();
    bool is_sealed() const;
    fclass* internal_value() const { return _value; }

protected:
    fclass* const _value;
};

#endif
