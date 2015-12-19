#include <string.h>
#include "test-nametable.h"
#include <interpreter/class.h>
#include <interpreter/nametable.h>


NametableTestFixture::NametableTestFixture()
{
}

NametableTestFixture::~NametableTestFixture()
{

}

void NametableTestFixture::setUp()
{
}

void NametableTestFixture::tearDown()
{
}

/**
 * Tests root object creation 
 */
void NametableTestFixture::testConstruction()
{
    nametable<fclass> classTable;
    CPPUNIT_ASSERT( classTable.size() == 0 );
}

/**
 * Tests size method
 */
void NametableTestFixture::testSize()
{
    nametable<fclass> classTable;
    fclass root("object");
    classTable.set(&root);
    CPPUNIT_ASSERT( classTable.size() == 1 );
}

/**
 * Tests retrieval by name
 */
void NametableTestFixture::testGetByName()
{
    nametable<fclass> classTable;
    fclass root("object");
    classTable.set(&root);
    CPPUNIT_ASSERT( &root == classTable.get("object") );
}

/**
 * Tests retrieval of absent name
 */
void NametableTestFixture::testGetByNameAbsent()
{
    nametable<fclass> classTable;
    fclass root("object");
    classTable.set(&root);
    CPPUNIT_ASSERT( nullptr == classTable.get("blah") );
}


/**
 * Tests contains method (by name)
 */
void NametableTestFixture::testContainsByName()
{
    nametable<fclass> classTable;
    fclass root("object");
    classTable.set(&root);
    CPPUNIT_ASSERT( classTable.contains("object") );
    CPPUNIT_ASSERT( !classTable.contains("banana") );
}

/**
 * Tests contains method (by object)
 */
void NametableTestFixture::testContainsByRef()
{
    nametable<fclass> classTable;
    fclass root("object");
    fclass child("child",root);
    classTable.set(&root);
    CPPUNIT_ASSERT( classTable.contains(&root) );
    CPPUNIT_ASSERT( !classTable.contains(&child) );
}

void NametableTestFixture::testRejectDuplicate()
{
    fclass root("object");
    fclass one("thing",root);
    fclass two("thing",root);

    nametable<fclass> classTable;
    classTable.set(&root);
    classTable.set(&one);
    fclass* existing = classTable.set(&two);

    CPPUNIT_ASSERT( existing == &one );
    CPPUNIT_ASSERT( classTable.get("thing") == &one );
}

void NametableTestFixture::testAcceptDuplicate()
{
    fclass root("object");
    fclass one("thing",root);
    fclass two("thing",root);

    nametable<fclass> classTable;
    classTable.set(&root);
    classTable.set(&one);
    fclass* existing = classTable.set(&two,true);

    CPPUNIT_ASSERT( existing == &one );
    CPPUNIT_ASSERT( classTable.get("thing") == &two );
}

void NametableTestFixture::testSetNew()
{
    fclass root("object");
    nametable<fclass> classTable;
    CPPUNIT_ASSERT( classTable.set(&root) == nullptr );
    CPPUNIT_ASSERT( classTable.contains("object") );
    CPPUNIT_ASSERT( classTable.contains(&root) );
    CPPUNIT_ASSERT( classTable.get("object")==&root);
}

