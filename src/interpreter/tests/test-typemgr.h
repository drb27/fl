#ifndef TEST_TYPEMGR_H
#define TEST_TYPEMGR_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

class TypemgrTestFixture : public CppUnit::TestFixture
{
public:
    TypemgrTestFixture();
    virtual ~TypemgrTestFixture();

    void setUp();
    void tearDown();

    /** @name Test Cases */
    // @{
    void testConstruction();
    void testSameClassReturnedBuiltin();
    // @}

    /** \cond internal */
    CPPUNIT_TEST_SUITE( TypemgrTestFixture );
    CPPUNIT_TEST( testConstruction );
    CPPUNIT_TEST( testSameClassReturnedBuiltin );
    CPPUNIT_TEST_SUITE_END();
    /** \endcond */
};

#endif
