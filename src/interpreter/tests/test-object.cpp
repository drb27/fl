#include <string>
#include <memory>
#include "test-object.h"
#include <interpreter/object.h>
#include <inc/references.h>

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
    typespec tso("object",{});
    typespec tsl("list",{tso} );

    return new list_object( g_pContext, g_pContext->types()->lookup(tsl));
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

    typespec tsv("void",{});
    void_object* null = new void_object(g_pContext,g_pContext->types()->lookup(tsv));
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

