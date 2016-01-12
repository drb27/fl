#ifndef TEST_CONTEXT_H
#define TEST_CONTEXT_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

class contextTestFixture : public CppUnit::TestFixture
{
public:
    contextTestFixture();
    virtual ~contextTestFixture();

    void setUp();
    void tearDown();

    /** @name Test Cases */
    // @{
    void testConstruction();
    void testCopyable();
    // @}

    /** \cond internal */
    CPPUNIT_TEST_SUITE( contextTestFixture );
    CPPUNIT_TEST( testConstruction );
    CPPUNIT_TEST( testCopyable );
    CPPUNIT_TEST_SUITE_END();
    /** \endcond */
};

#endif
