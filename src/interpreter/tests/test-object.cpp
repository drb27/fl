#include <string>
#include <memory>
#include "test-object.h"
#include <interpreter/obj/list_object.h>
#include <interpreter/obj/void_object.h>
#include <inc/references.h>
#include <interpreter/builtins.h>

extern context* g_pContext;

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

list_object* ObjectTestFixture::empty_list()
{
    return new list_object( g_pContext );
}

/**
 * Tests root object creation 
 */
void ObjectTestFixture::testConstruction()
{
}

void ObjectTestFixture::testHighlyChunkedAppend()
{
    listref a = listref(empty_list());
    listref b = listref(empty_list());

    void_object* null = new void_object(g_pContext);
    objref pNull(null);

    a->append(pNull);
    a->append(pNull);
    a->append(pNull);

    b->append(pNull);
    b->append(pNull);
    b->append(pNull);

    CPPUNIT_ASSERT(a->chunks()==3);
    CPPUNIT_ASSERT(b->chunks()==3);

    list_object* c = new list_object(g_pContext,*a);
    c->append(b);

    CPPUNIT_ASSERT(c->size()==6);
}

