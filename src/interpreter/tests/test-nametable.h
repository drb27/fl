#ifndef TEST_NAMETABLE_H
#define TEST_NAMETABLE_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

class NametableTestFixture : public CppUnit::TestFixture
{
public:
    NametableTestFixture();
    virtual ~NametableTestFixture();

    void setUp();
    void tearDown();

    /** @name Test Cases */
    // @{
    void testConstruction();
    void testSize();
    void testGetByName();
    void testGetByNameAbsent();
    void testContainsByName();
    void testContainsByRef();
    void testRejectDuplicate();
    void testAcceptDuplicate();
    void testSetNew();
    // @}

    /** \cond internal */
    CPPUNIT_TEST_SUITE( NametableTestFixture );
    CPPUNIT_TEST( testConstruction );
    CPPUNIT_TEST( testSize );
    CPPUNIT_TEST( testGetByName );
    CPPUNIT_TEST( testGetByNameAbsent );
    CPPUNIT_TEST( testContainsByName );
    CPPUNIT_TEST( testContainsByRef );
    CPPUNIT_TEST( testRejectDuplicate );
    CPPUNIT_TEST( testAcceptDuplicate );
    CPPUNIT_TEST( testSetNew );
    CPPUNIT_TEST_SUITE_END();
    /** \endcond */
};

#endif
