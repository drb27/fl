#ifndef TEST_PACKAGE_H
#define TEST_PACKAGE_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

class PackageTestFixture : public CppUnit::TestFixture
{
public:
    PackageTestFixture();
    virtual ~PackageTestFixture();

    void setUp();
    void tearDown();

    /** @name Test Cases */
    // @{
    void testConstruction();
    // @}

    /** \cond internal */
    CPPUNIT_TEST_SUITE( PackageTestFixture );
    CPPUNIT_TEST( testConstruction );
    CPPUNIT_TEST_SUITE_END();
    /** \endcond */
};

#endif
