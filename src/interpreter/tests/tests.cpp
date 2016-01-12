#include <iostream>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "test-fclass.h"
#include "test-nametable.h"
#include "test-object.h"
#include "test-typemgr.h"
#include "test-context.h"

CPPUNIT_TEST_SUITE_REGISTRATION(FclassTestFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(NametableTestFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(ObjectTestFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(TypemgrTestFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(contextTestFixture);

int main(int argc, char* argv[] )
{
    CppUnit::TextUi::TestRunner runner;
    CppUnit::Test* test = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    runner.addTest(test);
    bool wasSuccessful = runner.run();
    return (wasSuccessful)?0:1;
}
