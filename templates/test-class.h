#ifndef TEST_UPPERCLASS_H
#define TEST_UPPERCLASS_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

class CLASSTestFixture : public CppUnit::TestFixture
{
public:
    CLASSTestFixture();
    virtual ~CLASSTestFixture();

    void setUp();
    void tearDown();

    /** @name Test Cases */
    // @{
    void testConstruction();
    // @}

    /** \cond internal */
    CPPUNIT_TEST_SUITE( CLASSTestFixture );
    CPPUNIT_TEST( testConstruction );
    CPPUNIT_TEST_SUITE_END();
    /** \endcond */
};

#endif
