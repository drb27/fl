#include <string>
#include <vector>
#include "test-smartlist.h"
#include <interpreter/context.h>
#include <interpreter/object.h>
#include <interpreter/smartlist.h>

using std::vector;

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

chunkref smartlistTestFixture::make_chunk(size_t sz)
{
    vector<objref> items(sz);

    for (int i = 0 ; i < sz ; i++ )
	items[i] = objref( new int_object(g_pContext,i) );

    return chunk::make_singleblock_chunk(items);

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
	objref* o = c->_block.get()[i];
	intref expected = intref( new int_object(g_pContext,i) );
	CPPUNIT_ASSERT( (*expected)==*o );
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
