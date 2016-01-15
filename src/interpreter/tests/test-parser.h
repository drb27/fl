#ifndef TEST_PARSER_H
#define TEST_PARSER_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

class parserTestFixture : public CppUnit::TestFixture
{
public:
    parserTestFixture();
    virtual ~parserTestFixture();

    void setUp();
    void tearDown();

    /** @name Test Cases */
    // @{
    void testConstruction();
    // @}

    /** \cond internal */
    CPPUNIT_TEST_SUITE( parserTestFixture );
    CPPUNIT_TEST( testConstruction );
    CPPUNIT_TEST_SUITE_END();
    /** \endcond */
};

#endif
