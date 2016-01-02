#ifndef ACTION_TARGET_H
#define ACTION_TARGET_H

#include <string>
#include <iostream>

class ast;

class action_target
{
 
 public:

    virtual ast* make_int(int x) const=0;
    virtual ast* make_fundef( std::string* name, ast* def) const=0;
    virtual void respond( ast* def, std::ostream& os = std::cout ) const=0;
};

#endif
