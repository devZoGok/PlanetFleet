#include "mapTest.h"
#include "map.h"

namespace battleship{
	using namespace vb01;

	void MapTest::setUp(){
	}

	void MapTest::tearDown(){
	}

	void MapTest::testGenerateWeights(){
		Map *map = Map::getSingleton();
		Vector3 mapSize = map->getSize(), cellSize = Vector3(6, 1, 6);
		int cellsByDim[3] = { 
			int(mapSize.x / cellSize.x),
		   	int(mapSize.y / cellSize.y),
		   	int(mapSize.z / cellSize.z)
		};
		int numCells = cellsByDim[0] * cellsByDim[1] * cellsByDim[2];

		u32 **weights = new u32*[numCells];

		for(int i = 0; i < numCells; i++)
			weights[i] = new u32[numCells];

		map->generateWeights(weights, cellSize, UnitType::UNDERWATER, Vector3::VEC_ZERO, 60000);
	}
}
