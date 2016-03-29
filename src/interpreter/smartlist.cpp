#include <iostream>
#include <string>
#include <cassert>
#include <interpreter/eval_exception.h>
#include "smartlist.h"

using std::vector;

chunk::chunk(size_t size, size_t head, blockref& block, chunkref& chunk)
    : _size(size), _block(block), _next(chunk), _idx_head(head)
{
}

chunk::chunk( const chunk& other )
{
    _size = other._size;
    _block = other._block;
    _next = other._next;
    _idx_head = other._idx_head;
}

blockref chunk::make_block(size_t size)
{
    return blockref(new objref[size], [](objref* oa){ delete [] oa;} );
}

blockref chunk::copy_block( blockref src, size_t size )
{
    // Make a new block
    blockref br = make_block(size);

    // Copy each of the objrefs
    int index=0;
    for ( objref* pObjRef = src.get(); pObjRef< src.get()+size; pObjRef++ )
    {
	br.get()[index++] = objref(*pObjRef);
    }

    return br;
}

void chunk::copy_block( blockref src, blockref dst, size_t idxSrc, size_t idxDst, size_t length)
{
    // Copy each of the objrefs
    for ( objref* pSrcRef = src.get()+idxSrc; pSrcRef< src.get()+idxSrc+length; pSrcRef++ )
    {
	dst.get()[idxDst++] = objref(*pSrcRef);
    }

}

blockref chunk::copy_block( blockref src, size_t idxSrc, size_t length)
{
    // Make a new block
    blockref br = make_block(length);

    // Copy each of the objrefs
    size_t idxDst=0;
    for ( objref* pSrcRef = src.get()+idxSrc; pSrcRef< src.get()+idxSrc+length; pSrcRef++ )
    {
	br.get()[idxDst++] = objref(*pSrcRef);
    }

    return br;
}

blockref chunk::make_block( const vector<objref>& items )
{
    // Make a new block
    blockref br = make_block( items.size() );

    // Copy the items into the block
    for ( int i = 0 ; i < items.size(); i++ )
    {
	br.get()[i] = objref(items[i]);
    }

    return br;
}

chunkref chunk::make_singleblock_chunk( const vector<objref>& items )
{
    // Make a block
    blockref br = make_block(items);
    
    // Set up the chunk
    chunkref nxt = chunkref(nullptr);
    chunkref cr = chunkref( new chunk(items.size(),0, br,nxt) );

    return chunkref(cr);
}

smartlist::smartlist()
{
}

smartlist::smartlist( chunkref c )
    : _chunk(c)
{
}

smartlist::~smartlist()
{

}

smartlist::smartlist(smartlist&& other) : smartlist()
{
    // Move the entire chunk chain over to the new object
    _chunk = other._chunk;
    other._chunk = nullptr;
}

smartlist::smartlist( const smartlist& other ) 
    : smartlist()
{
    // Check for the empty list
    if (other.size()==0)
	return;

    // Simply copy all of the chunks!
    auto currentOtherChunk = other._chunk;
    _chunk = chunkref( new chunk(*(currentOtherChunk.get())) );
    auto currentThisChunk = _chunk;

    while ( currentOtherChunk->_next )
    {
	currentOtherChunk = currentOtherChunk->_next;
	currentThisChunk->_next = chunkref(new chunk(*(currentOtherChunk.get())));
	currentThisChunk = currentThisChunk->_next;
    };
}

smartlist& smartlist::operator=(smartlist&& other)
{
    // Don't bother moving an object to itself
    if (this!=&other)
    {
	// Move the entire chunk chain across
	_chunk = other._chunk;
	other._chunk = nullptr;
    }

    return *this;
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

objref smartlist::head() const
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

chunkref smartlist::penultimate_chunk() const
{
    if (_chunk)
    {
	chunkref pCurrentChunk = _chunk;
	chunkref pPreviousChunk;

	while( pCurrentChunk->_next )
	{
	    pPreviousChunk = pCurrentChunk;
	    pCurrentChunk = pCurrentChunk->_next;
	}

	return pPreviousChunk;
    }
    else
	return chunkref(nullptr);
}

void smartlist::inplace_append(chunkref& c)
{
    size_t oldSize = size();

    auto tailchunk = tail_chunk();
 
    if ( !tailchunk )
    {
	_chunk = c;
    }
    else
    {
	tailchunk->_next = c;
    }

    assert( size()==(oldSize+c->_size));
}

void smartlist::inplace_append(smartlist* other)
{
    size_t oldSize = size();

    // Get the last chunk in this list
    chunkref pAppendChunk = tail_chunk();

    // Iterate through each of the chunks in the other list
    chunkref pCurrentChunk = other->_chunk;

    while (pCurrentChunk)
    {
	// Place a copy of the current chunk in the other list onto the
	// end of the append chunk
	if (pAppendChunk)
	    pAppendChunk->_next = chunkref( new chunk(*pCurrentChunk) );
	else
	{
	    _chunk = chunkref( new chunk(*pCurrentChunk) );
	}
	   

	// Update pointers for next iteration
	if (pAppendChunk)
	    pAppendChunk = pAppendChunk->_next;
	else
	    pAppendChunk = _chunk;

	pCurrentChunk = pCurrentChunk->_next;
    }

    assert( size() == (oldSize+other->size()));
}

void smartlist::inplace_append(blockref& b,size_t s)
{
    size_t oldSize = size();
    auto n = chunkref(nullptr);
    auto newChunk = chunkref( new chunk(s,0,b,n) );
    inplace_append( newChunk );
    
    assert(size()==(s+oldSize));
}

void smartlist::inplace_append(const objref& o )
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

void smartlist::detach()
{
    // Iterate through the chunks, copying blocks which are not unique
    chunkref currentChunk = _chunk;
    chunkref previousChunk = chunkref(nullptr);

    while (currentChunk)
    {
	if ( !currentChunk->_block.unique() )
	{
	    currentChunk->_block = chunk::copy_block(currentChunk->_block,currentChunk->_size);
	}
	previousChunk = currentChunk;
	currentChunk = currentChunk->_next;
    }
}

objref smartlist::get_element(size_t index) const
{
    // Bounds check
    if ( (index >= size()) || (index < 0 ) )
	return objref(nullptr);

    // Now we must have a chunk
    auto currentChunk = _chunk;
    int startingIndex=0;

    // Iterate through the chunks until we get to the right block
    do
    {
	if ( ( currentChunk->_size + startingIndex ) > index )
	{
	    // The result is in this chunk
	    int translatedIndex = currentChunk->_idx_head + ( index - startingIndex );
	    return currentChunk->_block.get()[translatedIndex];
	}
	else
	{
	    // Move on to the next chunk
	    startingIndex = startingIndex + currentChunk->_size;
	    currentChunk = currentChunk->_next;
	}

    } while (true);
}

smartlist* smartlist::tail() const
{
    // Create a shallow copy
    auto pNewList = new smartlist(*this);

    if ( pNewList->_chunk )
    {
	if ( pNewList->_chunk->_size >= 2 )
	{
	    pNewList->_chunk->_idx_head++;
	    pNewList->_chunk->_size--;
	}
	else
	{
	    // Either 1 or zero elements in this chunk
	    if ( pNewList->_chunk->_size == 1 )
	    {
		pNewList->_chunk = pNewList->_chunk->_next;
	    }
	    else
	    {
		// Internal error - chunk of size zero detected!
		throw eval_exception(cerror::internal_error, "Zero chunk size detected in smartlist::tail");
	    }
	}
    }

    return pNewList;
}

void smartlist::inplace_chunkify(size_t chunkSize)
{
    const size_t newBlockSize = size();

    if (!chunkSize)
	return;

    if ( chunkSize > newBlockSize )
	throw eval_exception(cerror::internal_error, "Invalid block size in inplace_chunkify");

    // First, copy into one large block
    chunkref nullChunk = chunkref(nullptr);
    blockref newBlock = chunk::make_block(newBlockSize);

    // Copy each chunk's data into the monolith
    auto currentChunk = _chunk;
    int startingIndex = 0;

    while(currentChunk)
    {
	chunk::copy_block(currentChunk->_block, newBlock, currentChunk->_idx_head, startingIndex, currentChunk->_size);
	startingIndex+=currentChunk->_size;
	currentChunk = currentChunk->_next;
    }

    // Now split into individual chunks, preserving the single block
    const size_t firstChunkSize = newBlockSize%chunkSize;
    chunkref newHead = chunkref( new chunk(firstChunkSize,
					   0,
					   newBlock,
					   nullChunk) );

    size_t itemsRemaining = newBlockSize - firstChunkSize;
    size_t nextHeadIndex = firstChunkSize;

    currentChunk = newHead;

    while (itemsRemaining)
    {
	currentChunk->_next = chunkref ( new chunk(chunkSize,nextHeadIndex,newBlock,nullChunk) );
	itemsRemaining -= chunkSize;
	nextHeadIndex += chunkSize;
	currentChunk = currentChunk->_next;
    }

    // Make the new chunk our head chunk, or skip it if it is zero sized
    if ( newHead->_size)
	_chunk = newHead;
    else
	_chunk = newHead->_next;
}

size_t smartlist::chunks() const
{
    auto currentChunk = _chunk; 
    size_t count=0;

    while(currentChunk)
    {
	count++;
	currentChunk = currentChunk->_next;
    }

    return count;
}

objref smartlist::inplace_pop()
{
    chunkref pTailChunk;
    objref pReturnObject;

    if (_chunk && (_chunk->_size>0))
    {
	pTailChunk = tail_chunk();
	if ( !pTailChunk->_size)
	{
	    chunkref pPenultimateChunk = penultimate_chunk();
	    pPenultimateChunk->_next = chunkref(nullptr);
	    pTailChunk = pPenultimateChunk;
	}
    }

    if (pTailChunk)
    {
	int correctedIndex = pTailChunk->_idx_head + pTailChunk->_size - 1;
	pReturnObject = pTailChunk->_block.get()[correctedIndex];
	pTailChunk->_size--;
    }

    return pReturnObject;
}

void smartlist::dump_chunks()
{
    auto pCurrentChunk = _chunk;

    while(pCurrentChunk)
    {
	std::cout << pCurrentChunk->_size << "->";
	pCurrentChunk = pCurrentChunk->_next;
    }

    std::cout << "NULL" << std::endl;
}

chunkref smartlist::get_chunk_containing_index(size_t idx) const
{
    return make_ref(idx)._chunk;
}

smartlist smartlist::slice(size_t firstIndex, size_t lastIndex) const
{
    const size_t mySize = size();

    // If our size is zero, any slice must be the empty list
    if (!mySize)
	return smartlist();

    // If lastIndex<firstIndex, the slice is the empty list
    if ( lastIndex<firstIndex )
	return smartlist();

    // If both indices are out of bounds, return the empty list
    if ( (firstIndex>=mySize) )
	return smartlist();

    // Clamp the indices to our own bounds
    if ( firstIndex>=mySize )
	firstIndex=mySize-1;

    if (lastIndex>=mySize )
	lastIndex = mySize-1;

    auto firstChunkRef = make_ref(firstIndex);
    auto lastChunkRef = make_ref(lastIndex);

    // Copy the chunk chain from firstChunk to lastChunk 

    auto newFirstChunk(chunkref( new chunk(*(firstChunkRef._chunk)) ));
    newFirstChunk->_next=nullptr;

    auto currentNewChunk = newFirstChunk;
    auto currentOldChunk = firstChunkRef._chunk;
 
    while ( currentOldChunk != (lastChunkRef._chunk) )
    {
	currentOldChunk = currentOldChunk->_next;
	currentNewChunk->_next = chunkref( new chunk(*currentOldChunk) );
	currentNewChunk = currentNewChunk->_next;
	currentNewChunk->_next = nullptr;
    }

    // Adjust the end chunk index to point to the right place
    currentNewChunk->_size = 1+ lastChunkRef._offset;

    // Adjust the first chunk index to point to the right place
    newFirstChunk->_idx_head += firstChunkRef._offset;
    newFirstChunk->_size -= firstChunkRef._offset;

    // Make a new smartlist with this chunk chain
    return smartlist(newFirstChunk);

}

auto smartlist::make_ref(size_t idx) const -> ref
{
    ref retVal;

    // Bounds check
    if ( (idx >= size()) || (idx < 0 ) )
	return retVal;

    // Now we must have a chunk
    auto currentChunk = _chunk;
    int startingIndex=0;

    // Iterate through the chunks until we get to the right one
    do
    {
	if ( ( currentChunk->_size + startingIndex) > idx )
	{
	    // The result must be in this chunk
	    break;
	}
	else
	{
	    // Move on to the next chunk
	    startingIndex = startingIndex + currentChunk->_size;
	    currentChunk = currentChunk->_next;
	}
    } while(true);

    // Broken out of the look - currentChunk is the one we need
    retVal._chunk = currentChunk;
    retVal._offset = idx - ( startingIndex + currentChunk->_idx_head);

    return retVal;
}

void smartlist::ref::validate() const 
{
    if ( _chunk && ( ( (_chunk->_idx_head + _offset) > _chunk->_size )) )
	throw std::logic_error("Invalid chunk reference");
}

void smartlist::uniquify_block( chunkref chk )
{
    chk->_block = chunk::copy_block(chk->_block,chk->_idx_head, chk->_size);
    chk->_idx_head=0;
}

void smartlist::inplace_replace(size_t index, const objref& newValue)
{
    // Bounds check
    if (index>=size())
	throw std::logic_error("Smartlist index out of bounds");

    // Get the chunk that contains the element being replaced
    ref repRef = make_ref(index);

    // Is this a shared chunk?
    if ( !repRef._chunk->_block.unique() )
    {
	// Yes - we must splice in a copy of the block to be modified first
	uniquify_block(repRef._chunk);
    }

    // Now we are guaranteed to have a unique block, we can modify it directly
    repRef._chunk->_block.get()[ repRef._chunk->_idx_head + repRef._offset ] = newValue;

}
