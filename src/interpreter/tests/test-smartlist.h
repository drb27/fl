#ifndef TEST_SMARTLIST_H
#define TEST_SMARTLIST_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <interpreter/smartlist.h>

class smartlistTestFixture : public CppUnit::TestFixture
{
public:
    smartlistTestFixture();
    virtual ~smartlistTestFixture();

    void setUp();
    void tearDown();

    chunkref make_chunk(size_t, int start=0);
    blockref make_block(size_t sz, int start=0);
    void configure_shared_lists(smartlist&,smartlist&);

    /** @name Test Cases */
    // @{
    void testConstruction();
    void testSizeZero();
    void testMakeSingleblockChunk();
    void testIpAppendChunkToEmpty();
    void testIpAppendChunkToExisting();
    void testIpPrefixChunkToEmpty();
    void testIpPrefixChunkToExisting();
    void testUniqueEmpty();
    void testUniqueUnique();
    void testUniqueNotUnique();
    void testDetach();
    void testGetElement();
    void testShallowCopyConstructor();
    void testTail();
    void testChunkCount();
    void testCopyBlockSub();
    // @}

    /** \cond internal */
    CPPUNIT_TEST_SUITE( smartlistTestFixture );
    CPPUNIT_TEST( testConstruction );
    CPPUNIT_TEST( testSizeZero );
    CPPUNIT_TEST( testMakeSingleblockChunk);
    CPPUNIT_TEST( testIpAppendChunkToEmpty );
    CPPUNIT_TEST( testIpAppendChunkToExisting );
    CPPUNIT_TEST( testIpPrefixChunkToEmpty );
    CPPUNIT_TEST( testIpPrefixChunkToExisting );
    CPPUNIT_TEST( testUniqueEmpty );
    CPPUNIT_TEST( testUniqueUnique );
    CPPUNIT_TEST( testUniqueNotUnique );
    CPPUNIT_TEST( testDetach );
    CPPUNIT_TEST( testGetElement );
    CPPUNIT_TEST( testShallowCopyConstructor );
    CPPUNIT_TEST( testTail );
    CPPUNIT_TEST( testChunkCount );
    CPPUNIT_TEST( testCopyBlockSub );
    CPPUNIT_TEST_SUITE_END();
    /** \endcond */
};

#endif
