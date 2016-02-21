#include "test-package.h"
#include <string>
#include <interpreter/package.h>
#include <inc/references.h>
#include <interpreter/obj/int_object.h>

using std::string;

extern context* g_pContext;

packageTestFixture::packageTestFixture()
{
}

packageTestFixture::~packageTestFixture()
{

}

void packageTestFixture::setUp()
{
    pRoot = new package("root",nullptr);
    pRoot->add_child("sub1");
    pRoot->add_child("sub2");

    auto pSub1 = pRoot->get_child("sub1");
    auto pSub2 = pRoot->get_child("sub2");

    pSub1->add_child("subsub");
    pSub2->add_child("subsub");

    intref one = (intref)(new int_object(g_pContext,1));
    intref two = (intref)(new int_object(g_pContext,2));

    pSub1->get_child("subsub")->assign(string("x"),one);
    pSub2->get_child("subsub")->assign(string("x"),two);
}

void packageTestFixture::tearDown()
{
}

/**
 * Tests root object creation 
 */
void packageTestFixture::testConstruction()
{
    CPPUNIT_ASSERT(pRoot->name()=="root");
    CPPUNIT_ASSERT(pRoot->parent()==nullptr);

    CPPUNIT_ASSERT(pRoot->get_child("sub1")->name()=="sub1");
    CPPUNIT_ASSERT(pRoot->get_child("sub1")->parent()==pRoot);
}

void packageTestFixture::testIsRoot()
{
    CPPUNIT_ASSERT( pRoot->is_root() );
    CPPUNIT_ASSERT( !(pRoot->get_child("sub1")->is_root()) );
}

