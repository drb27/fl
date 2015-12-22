#ifndef OBJECT_H
#define OBJECT_H

#include <map>
#include <list>
#include <ostream>
#include "class.h"

class object
{

 public:

    object(const fclass&);
    virtual ~object();

    const fclass& get_class() const { return _class; }
    bool has_attribute(const std::string&) const;
    virtual void render( std::ostream& os ) const;

 protected:

 private:
    const fclass& _class;
    std::map<std::string,object*> _attributes;
};

#endif
