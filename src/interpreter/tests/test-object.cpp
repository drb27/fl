#include <string.h>
#include "test-object.h"
#include <interpreter/object.h>


ObjectTestFixture::ObjectTestFixture()
{
}

ObjectTestFixture::~ObjectTestFixture()
{

}

void ObjectTestFixture::setUp()
{
}

void ObjectTestFixture::tearDown()
{
}

/**
 * Tests root object creation 
 */
void ObjectTestFixture::testConstruction()
{
    object o(object::root_object_class());
    ftinteger i(0);

    CPPUNIT_ASSERT( o.get_class().name() == "object");
    CPPUNIT_ASSERT( i.get_class().name() == "integer");
    CPPUNIT_ASSERT( &i.get_class().base() == &o.get_class() );
}

