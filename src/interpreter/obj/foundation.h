#ifndef OBJ_FOUNDATION_H
#define OBJ_FOUNDATION_H

#include <interpreter/obj/object.h>

template<typename T>
class foundation_object : public object
{
public:
    foundation_object( context* pContext, T& value, fclass& cls)
	: object(pContext,cls), _value(value) {}

    T internal_value() const { return _value; }
    void set_internal_value(T v) { _value=v; }

    virtual void render( std::ostream& os, bool abbrev=true)
    {
	os << _value << " ";
	object::render(os,abbrev);
    }

    virtual bool operator==(const objref other) const
    {
	if (&other->get_class()!=&get_class())
	    return false;

	fref o = std::dynamic_pointer_cast<foundation_object<T>>(other);
	return (_value==o->_value);
    }

protected:
    T _value;

private:

    typedef std::shared_ptr<foundation_object<T>> fref;
};

#endif
