#include <string>
#include <cassert>
#include "fastlist.h"


fastlist::fastlist(size_t initialSize)
{
    _block = new blockref( new objref[initialSize], [](const objref* o) { delete[] o; });
    _idx_head=0;
    _idx_end=0;
}

fastlist::fastlist(fastlist& other, size_t newStart, size_t newEnd)
{
    _block = other._block;
    _idx_head=newStart;
    _idx_end=newEnd;
}

inline size_t fastlist::size() const
{
    return _idx_end - _idx_head;
}

fastlist::~fastlist()
{
    delete _block;
}

inline void fastlist::push_back(objref* o )
{
    if ( ! (_block->unique()) )
    {
	clone_block();
    }

    assert(_block->unique() );
}

inline void fastlist::push_front(objref* o )
{

}

inline objref* fastlist::get(size_t index)
{
    return nullptr;
}

inline void fastlist::clone_block()
{
    // Make a copy of the block between _idx_head and _idx_end-1
    const size_t newSize = size();
    auto newBlock = new blockref( new objref[newSize], [](const objref* o) { delete[] o; });
 
    // Copy the pointers in the array
    for (int idx = _idx_head; idx < _idx_end; idx++)
    {
	*(newBlock->get() + idx - _idx_head) = *(_block->get() + idx);
    }

    // Update our reference
    _block = newBlock;
   
}
