#include <string.h>
#include "test-typemgr.h"
#include <interpreter/typemgr.h>

TypemgrTestFixture::TypemgrTestFixture()
{
}

TypemgrTestFixture::~TypemgrTestFixture()
{

}

void TypemgrTestFixture::setUp()
{
}

void TypemgrTestFixture::tearDown()
{
}

/**
 * Tests root object creation 
 */
void TypemgrTestFixture::testConstruction()
{
    typemgr tm;
}

void TypemgrTestFixture::testSameClassReturnedBuiltin()
{
    typemgr tm;
    typespec int_spec("integer",{});
    typespec int_spec2("integer",{});

    const fclass& cls1 = tm.lookup(int_spec);
    const fclass& cls2 = tm.lookup(int_spec2);

    CPPUNIT_ASSERT( &cls1 == &cls2 );
}

