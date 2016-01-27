#include <iostream>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <logger/logger.h>
#include <interpreter/context.h>

#include "test-fclass.h"
#include "test-object.h"
#include "test-typemgr.h"
#include "test-context.h"
#include "test-parser.h"
#include "test-smartlist.h"

logger g_logger(std::cout);
context* g_pContext;

CPPUNIT_TEST_SUITE_REGISTRATION(FclassTestFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(ObjectTestFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(TypemgrTestFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(contextTestFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(parserTestFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(smartlistTestFixture);

int main(int argc, char* argv[] )
{
    g_pContext = new context();
    CppUnit::TextUi::TestRunner runner;
    CppUnit::Test* test = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    runner.addTest(test);
    bool wasSuccessful = runner.run();
    delete g_pContext;
    return (wasSuccessful)?0:1;
}
