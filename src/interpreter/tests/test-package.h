#ifndef TEST_PACKAGE_H
#define TEST_PACKAGE_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

class package;

class packageTestFixture : public CppUnit::TestFixture
{
public:
    packageTestFixture();
    virtual ~packageTestFixture();

    void setUp();
    void tearDown();

    /** @name Test Cases */
    // @{
    void testConstruction();
    void testIsRoot();
    // @}

    /** \cond internal */
    CPPUNIT_TEST_SUITE( packageTestFixture );
    CPPUNIT_TEST( testConstruction );
    CPPUNIT_TEST( testIsRoot );
    CPPUNIT_TEST_SUITE_END();
    /** \endcond */

 protected:

    package* pRoot;
};

#endif
