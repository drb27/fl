#ifndef MARSHALL_H
#define MARSHALL_H

#include <string>
#include <vector>
#include <map>


class fclass;

typedef std::pair<std::string,fclass*> argument;

struct sigbase
{
    std::string name;
    fclass* returnType;
};

template<int A>
struct signature : public sigbase
{
    std::array<argument,A> args;
};

template<int A>
struct fndata : public signature<A>
{
    void* pFn;
};



class marshall
{

 public:
    marshall();
    virtual ~marshall();

 protected:

 private:

};

#endif
