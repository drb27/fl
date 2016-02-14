#ifndef OBJ_LIST_H
#define OBJ_LIST_H

#include <interpreter/obj/object.h>

class smartlist;

class list_object : public object
{
public:

    list_object(context*,
		fclass& = *builtins::list::get_class() );

    list_object(context*,
		std::list<objref> startingList, 
		fclass& = *builtins::list::get_class());

    list_object(context*,
		smartlist*,
		fclass& = *builtins::list::get_class() );
    
    list_object(context*,const list_object&);
    
    virtual void render( std::ostream& os, bool abbrev=true);
    virtual void optimise();

    objref first();
    void append(objref e);
    void append(listref);
    void prepend(objref e);
    objref pop();
    objref get_element(size_t index);
    int size() const;
    int chunks() const;
    listref tail(context*) const;

protected:
    std::shared_ptr<smartlist> _pList;
};

#endif
