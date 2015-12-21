#include <string.h>
#include "test-package.h"
#include <interpreter/package.h>


PackageTestFixture::PackageTestFixture()
{
}

PackageTestFixture::~PackageTestFixture()
{

}

void PackageTestFixture::setUp()
{
}

void PackageTestFixture::tearDown()
{
}

/**
 * Tests root object creation 
 */
void PackageTestFixture::testConstruction()
{
    package p("root");
    CPPUNIT_ASSERT( p.name()=="root" );
    CPPUNIT_ASSERT( p.is_root() );
}

