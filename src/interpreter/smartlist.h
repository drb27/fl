#ifndef SMARTLIST_H
#define SMARTLIST_H

#include <memory>
#include <vector>
#include <interpreter/marshall.h>

class chunk;

typedef std::shared_ptr<chunk> chunkref;
typedef std::shared_ptr<objref> blockref;

class chunk final
{
 public:
    chunk(size_t,size_t,blockref&,chunkref&);
    chunk(const chunk&);

    static chunkref make_singleblock_chunk( const std::vector<objref>& );
    static blockref make_block( const std::vector<objref>& );
    static blockref make_block( size_t size);
    static blockref copy_block( blockref src, size_t size );
    static void copy_block( blockref src, blockref dst, size_t idxSrc, size_t idxDst, size_t length);
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

    smartlist( const smartlist& other);
    smartlist( chunkref );

    size_t size() const;
    size_t chunks() const;
    objref head() const;

    void inplace_append(smartlist* other);
    void inplace_append(blockref&,size_t);
    void inplace_append(chunkref&);
    void inplace_append(objref&);

    void inplace_prefix(blockref&,size_t);
    void inplace_prefix(chunkref&);
    void inplace_prefix(objref&);

    objref get_element(size_t) const;
    smartlist* tail() const;

    void inplace_chunkify(size_t blockSize=0);

    bool unique() const;
    void detach();

    void dump_chunks();

 protected:

    inline chunkref tail_chunk() const;


 private:
    chunkref _chunk{nullptr};
};

#endif
