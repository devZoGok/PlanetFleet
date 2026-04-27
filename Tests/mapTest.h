#ifndef MAP_TEST_H
#define MAP_TEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

namespace battleship{
		class MapTest : public CppUnit::TestFixture{
				CPPUNIT_TEST_SUITE(MapTest);
				CPPUNIT_TEST(testGenerateWeights);
				CPPUNIT_TEST_SUITE_END();

				public:
						MapTest(){}
						void testGenerateWeights();
						void setUp();
						void tearDown();
		};
}

#endif
