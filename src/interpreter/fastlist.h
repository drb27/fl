#ifndef FASTLIST_H
#define FASTLIST_H

#include <memory>
#include <interpreter/marshall.h>

typedef std::shared_ptr<objref> blockref;

class fastlist final
{

 public:
    fastlist(size_t initialSize);
    fastlist( fastlist& other, size_t newStart, size_t newEnd);
    ~fastlist();
    inline size_t size() const;

    inline void push_back(objref*);
    inline void push_front(objref*);

    inline objref* get(size_t);

 private:

    inline void clone_block();

    size_t _idx_head;
    size_t _idx_end;
    blockref* _block;
};

#endif
