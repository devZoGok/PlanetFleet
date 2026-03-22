#include <cppunit/TestSuite.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>

#include "pathfinderTest.h"

using namespace CppUnit;
using namespace battleship;

int main(){
	TextUi::TestRunner runner;
	runner.addTest(PathfinderTest::suite());
	runner.run();
	return 0;
}
