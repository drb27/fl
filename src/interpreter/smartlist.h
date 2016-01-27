#ifndef SMARTLIST_H
#define SMARTLIST_H

#include <memory>
#include <vector>
#include <interpreter/marshall.h>

typedef objref* objrefptr;

class chunk;

typedef std::shared_ptr<chunk> chunkref;
typedef std::shared_ptr<objref*> blockref;

class chunk final
{
 public:
    chunk(size_t,size_t,blockref&,chunkref&);

    static chunkref make_singleblock_chunk( const std::vector<objref>& );

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

    void inplace_append(blockref&,size_t);
    void inplace_append(chunkref&);
    void inplace_append(objref&);

    void inplace_prefix(blockref&,size_t);
    void inplace_prefix(chunkref&);
    void inplace_prefix(objref&);

    bool unique() const;

 protected:

    inline chunkref tail_chunk() const;


 private:
    chunkref _chunk{nullptr};
};

#endif
