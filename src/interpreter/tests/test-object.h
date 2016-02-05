#ifndef TEST_OBJECT_H
#define TEST_OBJECT_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

class list_object;

class ObjectTestFixture : public CppUnit::TestFixture
{
public:
    ObjectTestFixture();
    virtual ~ObjectTestFixture();

    void setUp();
    void tearDown();
    list_object* empty_list();

    /** @name Test Cases */
    // @{
    void testConstruction();
    void testHighlyChunkedAppend();
    // @}

    /** \cond internal */
    CPPUNIT_TEST_SUITE( ObjectTestFixture );
    CPPUNIT_TEST( testConstruction );
    CPPUNIT_TEST( testHighlyChunkedAppend );
    CPPUNIT_TEST_SUITE_END();
    /** \endcond */
};

#endif
