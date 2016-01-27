#include <string>
#include "smartlist.h"

chunk::chunk(size_t size, size_t head, blockref& block, chunkref& chunk)
    : _size(size), _block(block), _next(chunk), _idx_head(head)
{

}

smartlist::smartlist()
{
}

smartlist::~smartlist()
{

}

size_t smartlist::size() const
{
    chunk* pCurrentChunk = _chunk.get();
    size_t cumulativeSize=0;

    while( pCurrentChunk )
    {
	cumulativeSize+= pCurrentChunk->_size;
	pCurrentChunk = pCurrentChunk->_next.get();
    }

    return cumulativeSize;
}

objref* smartlist::head() const
{
    return (*(_chunk->_block.get()))[0];
}

void smartlist::append(blockref& b,size_t s)
{
    chunk* pCurrentChunk = _chunk.get();

    while( pCurrentChunk->_next )
    {
	pCurrentChunk = pCurrentChunk->_next.get();
    }

    auto n = chunkref(nullptr);
    pCurrentChunk->_next = chunkref( new chunk(s,0,b,n) );
    
}
