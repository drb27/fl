#include <string>
#include <vector>
#include <memory>
#include "test-smartlist.h"
#include <interpreter/context.h>
#include <interpreter/obj/int_object.h>
#include <interpreter/obj/void_object.h>
#include <interpreter/smartlist.h>
#include <interpreter/builtins.h>

using std::vector;
using std::dynamic_pointer_cast;

extern context* g_pContext;

smartlistTestFixture::smartlistTestFixture()
{
}

smartlistTestFixture::~smartlistTestFixture()
{

}

void smartlistTestFixture::setUp()
{
}

void smartlistTestFixture::tearDown()
{
}

chunkref smartlistTestFixture::make_chunk(size_t sz,int start)
{
    vector<objref> items(sz);

    for (int i = 0 ; i < sz ; i++ )
	items[i] = objref( new int_object(g_pContext,start+i) );

    return chunk::make_singleblock_chunk(items);

}

blockref smartlistTestFixture::make_block(size_t sz, int start)
{
    vector<objref> items(sz);

    for (int i = 0 ; i < sz ; i++ )
	items[i] = objref( new int_object(g_pContext,start+i) );

    return chunk::make_block(items);
}

void smartlistTestFixture::configure_shared_lists(smartlist& a,smartlist& b)
{
    blockref b1 = make_block(3,1);
    blockref b2 = make_block(3,4);
    blockref b3 = make_block(3,7);
    blockref b4 = make_block(3,10);

    chunkref nullchunk( chunkref(nullptr) );
    chunkref c1 = chunkref( new chunk(3,0,b1,nullchunk) );
    chunkref c2 = chunkref( new chunk(3,0,b2,nullchunk) );
    chunkref c3 = chunkref( new chunk(3,0,b3,nullchunk) );
    chunkref c4 = chunkref( new chunk(3,0,b4,nullchunk) );
    chunkref c5 = chunkref( new chunk(3,0,b2,nullchunk) );

    a.inplace_append(c1);
    a.inplace_append(c2);
    a.inplace_append(c3);

    b.inplace_append(c4);
    b.inplace_append(c5);
}

/**
 * Tests root object creation 
 */
void smartlistTestFixture::testConstruction()
{
    smartlist s;
}

void smartlistTestFixture::testSizeZero()
{
    smartlist s;
    CPPUNIT_ASSERT( s.size()==0 );
}


void smartlistTestFixture::testMakeSingleblockChunk()
{
    chunkref c = make_chunk(5);

    CPPUNIT_ASSERT( c->_size == 5 );
    CPPUNIT_ASSERT( c->_idx_head == 0 );
    CPPUNIT_ASSERT( c->_block );
    CPPUNIT_ASSERT( c->_next == nullptr );

    for ( int i = 0 ; i < 5; i++ )
    {
	objref o = c->_block.get()[i];
	intref expected = intref( new int_object(g_pContext,i) );
	CPPUNIT_ASSERT( (*expected)==o );
    }
}

void smartlistTestFixture::testIpAppendChunkToEmpty()
{
    smartlist s;
    auto c = make_chunk(5);
    s.inplace_append( c );
    CPPUNIT_ASSERT( s.size()==5 );
}

void smartlistTestFixture::testIpAppendChunkToExisting()
{
    smartlist s;
    auto c = make_chunk(5);
    auto c2 = make_chunk(7);
    s.inplace_append( c );
    s.inplace_append( c2 );
    CPPUNIT_ASSERT( s.size()==12 );
}

void smartlistTestFixture::testIpPrefixChunkToEmpty()
{
    smartlist s;
    auto c = make_chunk(5);
    s.inplace_prefix( c );
    CPPUNIT_ASSERT( s.size()==5 );
}

void smartlistTestFixture::testIpPrefixChunkToExisting()
{
    smartlist s;
    auto c = make_chunk(5);
    auto c2 = make_chunk(7);
    s.inplace_prefix( c );
    s.inplace_prefix( c2 );
    CPPUNIT_ASSERT( s.size()==12 );
}

void smartlistTestFixture::testUniqueEmpty()
{
    smartlist s;
    CPPUNIT_ASSERT(s.unique());
}

void smartlistTestFixture::testUniqueUnique()
{
    smartlist s;
    auto c = make_chunk(5);
    s.inplace_append(c);
    CPPUNIT_ASSERT( s.unique() );
}

void smartlistTestFixture::testUniqueNotUnique()
{
    smartlist s1,s2;
    auto c = make_chunk(5);
    auto c2 = make_chunk(5);
    c2->_block = c->_block;
    s1.inplace_append(c);
    s2.inplace_append(c2);
    CPPUNIT_ASSERT( !s1.unique() );
    CPPUNIT_ASSERT( !s2.unique() );
}

void smartlistTestFixture::testDetach()
{
    smartlist a,b;
    configure_shared_lists(a,b);

    CPPUNIT_ASSERT( !a.unique() );
    CPPUNIT_ASSERT( !b.unique() );

    a.detach();

    for ( int index = 0 ; index < a.size(); index++ )
    {
	intref el = std::dynamic_pointer_cast<int_object>(a.get_element(index));
	CPPUNIT_ASSERT( el->internal_value() == 1+index );
    }

    CPPUNIT_ASSERT( a.unique() );
    CPPUNIT_ASSERT( b.unique() );
}

void smartlistTestFixture::testGetElement()
{
    smartlist a,b;
    configure_shared_lists(a,b);

    for ( int index = 0 ; index < a.size(); index++ )
    {
	intref el = std::dynamic_pointer_cast<int_object>(a.get_element(index));
	CPPUNIT_ASSERT( el->internal_value() == 1+index );
    }

    CPPUNIT_ASSERT( !a.unique() );
    CPPUNIT_ASSERT( !b.unique() );
}

void smartlistTestFixture::testShallowCopyConstructor()
{
    smartlist b,c;
    configure_shared_lists(b,c);

    smartlist a(b);
    for ( int index = 0 ; index < a.size(); index++ )
    {
	intref el = std::dynamic_pointer_cast<int_object>(a.get_element(index));
	CPPUNIT_ASSERT( el->internal_value() == 1+index );
    }
    CPPUNIT_ASSERT( !a.unique() );
    CPPUNIT_ASSERT( !b.unique() );
    CPPUNIT_ASSERT( !c.unique() );

    CPPUNIT_ASSERT( a.size()==b.size() );
    a.detach();

    for ( int index = 0 ; index < a.size(); index++ )
    {
	intref el = std::dynamic_pointer_cast<int_object>(a.get_element(index));
	CPPUNIT_ASSERT( el->internal_value() == 1+index );
    }

    CPPUNIT_ASSERT( a.unique() );
    CPPUNIT_ASSERT( a.size()==b.size() );
}

void smartlistTestFixture::testTail()
{
    smartlist a,b;
    configure_shared_lists(a,b);

    CPPUNIT_ASSERT(a.size()==9);
    for ( int index = 0 ; index < a.size(); index++ )
    {
	intref el = std::dynamic_pointer_cast<int_object>(a.get_element(index));
	CPPUNIT_ASSERT( el->internal_value() == 1+index );
    }

    smartlist* pC = a.tail();

    CPPUNIT_ASSERT(pC->size()==8);
    for ( int index = 0 ; index < pC->size(); index++ )
    {
	intref el = std::dynamic_pointer_cast<int_object>(pC->get_element(index));
	CPPUNIT_ASSERT( el->internal_value() == 2+index );
    }

    delete pC;

    CPPUNIT_ASSERT(a.size()==9);
    for ( int index = 0 ; index < a.size(); index++ )
    {
	intref el = std::dynamic_pointer_cast<int_object>(a.get_element(index));
	CPPUNIT_ASSERT( el->internal_value() == 1+index );
    }


}

void smartlistTestFixture::testChunkCount()
{
    smartlist a,b,c;
    configure_shared_lists(a,b);

    CPPUNIT_ASSERT( a.chunks()==3 );
    CPPUNIT_ASSERT( b.chunks()==2 );
    CPPUNIT_ASSERT( c.chunks()==0 );
}

void smartlistTestFixture::testCopyBlockSub()
{

    vector<objref> items(5);
    for ( int index=0; index < 5 ; index++ )
    {
	items[index] = objref( new int_object(g_pContext,index));
    }

    blockref brSrc = chunk::make_block(items);
    blockref brDst = chunk::make_block(2);

    chunk::copy_block(brSrc,brDst,2,0,2);
    chunkref nc = chunkref(nullptr);
    chunkref cr = chunkref( new chunk(2,0,brDst,nc) );
    smartlist s(cr);

    CPPUNIT_ASSERT( *(s.get_element(0)) == items[2] );
    CPPUNIT_ASSERT( *(s.get_element(1)) == items[3] );
    CPPUNIT_ASSERT( s.size() == 2);

}

void smartlistTestFixture::testInplaceChunkify()
{
    smartlist a,b;
    configure_shared_lists(a,b);

    // Test 1 - Monolith
    a.inplace_chunkify( a.size() );

    CPPUNIT_ASSERT(a.chunks()==1);
    CPPUNIT_ASSERT(a.size()==9);

    for ( int index=0; index < a.size(); index++ )
    {
	intref el = std::dynamic_pointer_cast<int_object>(a.get_element(index));
	CPPUNIT_ASSERT( el->internal_value() == 1+index );
    }

    // Test 2 - Perfect division
    smartlist c,d;
    configure_shared_lists(c,d);

    c.inplace_chunkify( 3 );

    CPPUNIT_ASSERT(c.chunks()==3);
    CPPUNIT_ASSERT(c.size()==9);

    for ( int index=0; index < c.size(); index++ )
    {
	intref el = std::dynamic_pointer_cast<int_object>(c.get_element(index));
	CPPUNIT_ASSERT( el->internal_value() == 1+index );
    }

    // Test 3 - Imperfect division
    smartlist e,f;
    configure_shared_lists(e,f);

    e.inplace_chunkify( 5 );

    CPPUNIT_ASSERT(e.chunks()==2);
    CPPUNIT_ASSERT(e.size()==9);

    for ( int index=0; index < e.size(); index++ )
    {
	intref el = std::dynamic_pointer_cast<int_object>(e.get_element(index));
	CPPUNIT_ASSERT( el->internal_value() == 1+index );
    }

}

void smartlistTestFixture::testChunkedTail()
{
    smartlist a,b;
    configure_shared_lists(a,b);

    smartlist* pNewList = a.tail();

    CPPUNIT_ASSERT( pNewList->size()==8);

    delete pNewList;
}

void smartlistTestFixture::testJoinChunkedToEmpty()
{
    objref null = objref(new void_object(g_pContext));
    smartlist a;

    smartlist b;
    b.inplace_append(null);    
    b.inplace_append(null);
    b.inplace_append(null);
    
    smartlist c(a);
    
    c.inplace_append(&b);

    CPPUNIT_ASSERT(c.size()==3);
}

void smartlistTestFixture::testInplacePop()
{
    smartlist a,b,c;
    configure_shared_lists(a,b);
    
    objref retVal;
    retVal = c.inplace_pop();
    CPPUNIT_ASSERT(!retVal);

    CPPUNIT_ASSERT(b.size()==6);
    retVal = b.inplace_pop();
    CPPUNIT_ASSERT( std::dynamic_pointer_cast<int_object>(retVal)->internal_value()==6);
    CPPUNIT_ASSERT( b.size()==5);

    retVal = b.inplace_pop();
    CPPUNIT_ASSERT( std::dynamic_pointer_cast<int_object>(retVal)->internal_value()==5);
    CPPUNIT_ASSERT( b.size()==4);

    retVal = b.inplace_pop();
    CPPUNIT_ASSERT( std::dynamic_pointer_cast<int_object>(retVal)->internal_value()==4);
    CPPUNIT_ASSERT( b.size()==3);

    retVal = b.inplace_pop();
    CPPUNIT_ASSERT( std::dynamic_pointer_cast<int_object>(retVal)->internal_value()==12);
    CPPUNIT_ASSERT( b.size()==2);

    retVal = b.inplace_pop();
    CPPUNIT_ASSERT( std::dynamic_pointer_cast<int_object>(retVal)->internal_value()==11);
    CPPUNIT_ASSERT( b.size()==1);

    retVal = b.inplace_pop();
    CPPUNIT_ASSERT( std::dynamic_pointer_cast<int_object>(retVal)->internal_value()==10);
    CPPUNIT_ASSERT( b.size()==0);
    
}

void smartlistTestFixture::testSliceEmpty()
{

    // Create an empty smartist
    smartlist l;

    // Attempt to take a zero-zero slice, check for the empty list
    smartlist slice = l.slice(0,0);
    CPPUNIT_ASSERT(slice.size()==0);

    // Check that modifying the new list does not affect the original one
    slice.inplace_append( objref(new void_object(g_pContext)) );
    CPPUNIT_ASSERT(slice.size()==1);
    CPPUNIT_ASSERT(l.size()==0);

    // Attempt to take a slice starting at zero
    slice = l.slice(0,100);
    CPPUNIT_ASSERT(l.size()==0);

    // Attempt to take a slice starting an non-zero
    slice = l.slice(1,10);
    CPPUNIT_ASSERT(l.size()==0);
}

void smartlistTestFixture::testSliceOutOfBounds()
{
    smartlist a,b;
    configure_shared_lists(a,b);

    // Attempt to create a slice which is completely out of limits
    smartlist l = a.slice(10,13);
    CPPUNIT_ASSERT(l.size()==0);

    // Attempt to create a slice which extends off the end of a single chunk
    l = a.slice(7,10);
    CPPUNIT_ASSERT(l.size()==2);
    CPPUNIT_ASSERT( dynamic_pointer_cast<int_object>(l.get_element(0))->internal_value() == 8 ); 
    CPPUNIT_ASSERT( dynamic_pointer_cast<int_object>(l.get_element(1))->internal_value() == 9 ); 

    // Attempt to create a slice which spans multiple chunks, then goes out of bounds
    l = a.slice(5,15);
    CPPUNIT_ASSERT(l.size()==4);
    CPPUNIT_ASSERT( dynamic_pointer_cast<int_object>(l.get_element(0))->internal_value() == 6 ); 
    CPPUNIT_ASSERT( dynamic_pointer_cast<int_object>(l.get_element(1))->internal_value() == 7 ); 
    CPPUNIT_ASSERT( dynamic_pointer_cast<int_object>(l.get_element(2))->internal_value() == 8 ); 
    CPPUNIT_ASSERT( dynamic_pointer_cast<int_object>(l.get_element(3))->internal_value() == 9 ); 
}

void smartlistTestFixture::testSliceSingleChunk()
{
    smartlist a,b;
    configure_shared_lists(a,b);

    // Whole chunk
    smartlist l = a.slice(0,2);
    CPPUNIT_ASSERT( l.size()==3 );
    CPPUNIT_ASSERT( dynamic_pointer_cast<int_object>(l.get_element(0))->internal_value() == 1 ); 
    CPPUNIT_ASSERT( dynamic_pointer_cast<int_object>(l.get_element(1))->internal_value() == 2 ); 
    CPPUNIT_ASSERT( dynamic_pointer_cast<int_object>(l.get_element(2))->internal_value() == 3 ); 

    // Middle of a chunk
    l = a.slice(1,1);
    CPPUNIT_ASSERT( l.size()==1 );
    CPPUNIT_ASSERT( dynamic_pointer_cast<int_object>(l.get_element(0))->internal_value() == 2 ); 

    // Start of a chunk
    l = a.slice(0,1);
    CPPUNIT_ASSERT( l.size()==2 );
    CPPUNIT_ASSERT( dynamic_pointer_cast<int_object>(l.get_element(0))->internal_value() == 1 ); 
    CPPUNIT_ASSERT( dynamic_pointer_cast<int_object>(l.get_element(1))->internal_value() == 2 ); 

    // End of a chunk
    l = a.slice(1,2);
    CPPUNIT_ASSERT( l.size()==2 );
    CPPUNIT_ASSERT( dynamic_pointer_cast<int_object>(l.get_element(0))->internal_value() == 2 ); 
    CPPUNIT_ASSERT( dynamic_pointer_cast<int_object>(l.get_element(1))->internal_value() == 3 ); 
}

void smartlistTestFixture::testSliceSpanChunks()
{
    smartlist a,b;
    configure_shared_lists(a,b);
    smartlist l = a.slice(1,5);
    CPPUNIT_ASSERT( l.size()==5 );
    CPPUNIT_ASSERT( dynamic_pointer_cast<int_object>(l.get_element(0))->internal_value() == 2 ); 
    CPPUNIT_ASSERT( dynamic_pointer_cast<int_object>(l.get_element(1))->internal_value() == 3 ); 
    CPPUNIT_ASSERT( dynamic_pointer_cast<int_object>(l.get_element(2))->internal_value() == 4 ); 
    CPPUNIT_ASSERT( dynamic_pointer_cast<int_object>(l.get_element(3))->internal_value() == 5 ); 
    CPPUNIT_ASSERT( dynamic_pointer_cast<int_object>(l.get_element(4))->internal_value() == 6 ); 
}
