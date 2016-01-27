#include <string>
#include "smartlist.h"

using std::vector;

chunk::chunk(size_t size, size_t head, blockref& block, chunkref& chunk)
    : _size(size), _block(block), _next(chunk), _idx_head(head)
{

}

chunkref chunk::make_singleblock_chunk( const vector<objref>& items )
{
    // Set up the chunk and associated block
    blockref br = blockref(new objrefptr[items.size()], [](objref** oa){ delete [] oa;} );
    chunkref nxt = chunkref(nullptr);
    chunkref cr = chunkref( new chunk(items.size(),0, br,nxt) );

    // Copy the items into the block
    for ( int i = 0 ; i < items.size(); i++ )
    {
	br.get()[i] = new objref(items[i]);
    }

    return chunkref(cr);
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
    if (_chunk)
    {
	return _chunk->_block.get()[0];
    }
    else
	return nullptr;
}

chunkref smartlist::tail_chunk() const
{
    if (_chunk)
    {
	chunkref pCurrentChunk = _chunk;
	while( pCurrentChunk->_next )
	{
	    pCurrentChunk = pCurrentChunk->_next;
	}

	return pCurrentChunk;
    }
    else
	return chunkref(nullptr);
}

void smartlist::inplace_append(chunkref& c)
{
    auto tailchunk = tail_chunk();
 
    if ( !tailchunk )
    {
	_chunk = c;
    }
    else
    {
	tailchunk->_next = c;
    }
}

void smartlist::inplace_append(blockref& b,size_t s)
{
    auto n = chunkref(nullptr);
    auto newChunk = chunkref( new chunk(s,0,b,n) );
    inplace_append( newChunk );
    
}

void smartlist::inplace_append(objref& o )
{
    vector<objref> items{o};
    chunkref c = chunk::make_singleblock_chunk(items);
    inplace_append(c);
}

///

void smartlist::inplace_prefix(chunkref& c)
{
    if (_chunk)
    {
	c->_next = _chunk;
	_chunk = c;
    }
    else
    {
	_chunk = c;
    }
}

void smartlist::inplace_prefix(blockref& b,size_t s)
{
    auto n = chunkref(nullptr);
    auto newChunk = chunkref( new chunk(s,0,b,n) );
    inplace_prefix( newChunk );
    
}

void smartlist::inplace_prefix(objref& o )
{
    vector<objref> items{o};
    chunkref c = chunk::make_singleblock_chunk(items);
    inplace_prefix(c);
}

bool smartlist::unique() const
{
    if (_chunk)
    {
	chunkref currentChunk = _chunk;
	while ( currentChunk )
	{
	    if ( !currentChunk->_block.unique() )
		return false;
	
	    currentChunk = currentChunk->_next;
	}
    }

    return true;
}
