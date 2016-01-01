#ifndef ACTION_TARGET_H
#define ACTION_TARGET_H

class ast;

class action_target
{
 
 public:

    virtual ast* make_int(int x) const=0;
    
};

#endif
