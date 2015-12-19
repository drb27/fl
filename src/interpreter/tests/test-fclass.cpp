#include <string.h>
#include "test-fclass.h"
#include <interpreter/class.h>
//#include <stocklib/buffer.h>

FclassTestFixture::FclassTestFixture()
{
}

FclassTestFixture::~FclassTestFixture()
{

}

void FclassTestFixture::setUp()
{
}

void FclassTestFixture::tearDown()
{
}

/**
 * Tests root object creation 
 */
void FclassTestFixture::testConstruction()
{
    fclass root("object");
    CPPUNIT_ASSERT(root.name()=="object");
}

/**
 * Tests non-root object creation
 */
void FclassTestFixture::testNonRootConstruction()
{
    fclass root("object");
    fclass child("child",root);

    CPPUNIT_ASSERT( &(child.base())==&root );
}
