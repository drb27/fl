#include <string.h>
#include "test-fclass.h"
#include <interpreter/class.h>

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
    typespec obj_spec("object",{});
    fclass root(obj_spec);
    CPPUNIT_ASSERT(root.name()=="object");
}

/**
 * Tests non-root object creation
 */
void FclassTestFixture::testNonRootConstruction()
{
    typespec obj_spec("object",{});
    typespec child_spec("child",{});
    
    fclass root(obj_spec);
    fclass child(child_spec,root);

    CPPUNIT_ASSERT( &(child.base())==&root );
}
