#ifndef SMARTLIST_H
#define SMARTLIST_H

#include <memory>
#include <interpreter/marshall.h>

typedef objref* block[];

class chunk;

typedef std::shared_ptr<chunk> chunkref;
typedef std::shared_ptr<block> blockref;

class chunk final
{
 public:
    chunk(size_t,size_t,blockref&,chunkref&);

    size_t _size;
    size_t _idx_head;
    blockref _block;
    chunkref _next;
};

class smartlist final
{

 public:
    smartlist();
    ~smartlist();

    size_t size() const;
    objref* head() const;

    void append(blockref&,size_t);

 protected:

 private:
    chunkref _chunk{nullptr};
};

#endif
