#include <string.h>
#include "test-context.h"
#include <interpreter/context.h>
#include <interpreter/object.h>

contextTestFixture::contextTestFixture()
{
}

contextTestFixture::~contextTestFixture()
{

}

void contextTestFixture::setUp()
{
}

void contextTestFixture::tearDown()
{
}

/**
 * Tests root object creation 
 */
void contextTestFixture::testConstruction()
{
    context c;
}

void contextTestFixture::testCopyable()
{
    context c;
    typespec int_spec("integer",{});

    intref a(new int_object(4,c.types().lookup(int_spec)));
    intref b(new int_object(6,c.types().lookup(int_spec)));

    c.assign("a",a);
    c.assign("b",b);

    context d(c);

    CPPUNIT_ASSERT( a==d.resolve_symbol("a"));
    CPPUNIT_ASSERT( b==d.resolve_symbol("b"));
    

}

