#ifndef TEST_FCLASS_H
#define TEST_FCLASS_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

class FclassTestFixture : public CppUnit::TestFixture
{
public:
    FclassTestFixture();
    virtual ~FclassTestFixture();

    void setUp();
    void tearDown();

    /** @name Test Cases */
    // @{
    void testConstruction();
    void testNonRootConstruction();
    // @}

    /** \cond internal */
    CPPUNIT_TEST_SUITE( FclassTestFixture );
    CPPUNIT_TEST( testConstruction );
    CPPUNIT_TEST( testNonRootConstruction );
    CPPUNIT_TEST_SUITE_END();
    /** \endcond */
};

#endif
