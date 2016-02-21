#ifndef OBJ_STRING_H
#define OBJ_STRING_H

#include <interpreter/obj/object.h>

class string_object : public object
{
public:

    string_object(context*,
		  const std::string&, 
		  fclass& = *builtins::string::get_class() );

    string_object( const string_object& );
    virtual void render( std::ostream& os, bool abbrev=true );
    const std::string& internal_value() const { return _value; }
    
    virtual stringref operator[](intref index) const;
    virtual bool operator==(const objref other) const; 
    
    virtual stringref operator+(const stringref other) const;
    virtual void inplace_join(const stringref other);
    
protected:
    std::string _value;
};

#endif
