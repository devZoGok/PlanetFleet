#include "pathfinderTest.h"
#include "pathfinder.h"

#include <vector.h>
#include <util.h>

#include <vector>

namespace battleship{
		using namespace std;
		using namespace vb01;

		vector<Map::Cell> PathfinderTest::generateCellGraph(int numCellsOnSide){
			vector<Map::Cell> cells;
			float cellSize = 2;

			for(int i = 0; i < numCellsOnSide; i++)
				for(int j = 0; j < numCellsOnSide; j++){
					vector<Map::Edge> edges = Map::generateAdjacentNodeEdges(numCellsOnSide, i, numCellsOnSide, j, 1000);
					cells.push_back(Map::Cell(Vector3(j * cellSize, 0, i * cellSize), Map::Cell::Type::LAND, edges));
				}

			return cells;
		}

		int PathfinderTest::calcPathLength(vector<int> &path){
			int sumPathWeights = 0;

			for(int i = 0; i < path.size() - 1; i++)
				for(int j = 0; j < cells[path[i]].edges.size(); j++)
					if(cells[path[i]].edges[j].destCellId == path[i + 1])
						sumPathWeights += cells[path[i]].edges[j].weight;

			return sumPathWeights;
		}

		vector<float> PathfinderTest::generateHeuristics(vector<Map::Cell> &cells, int dest, int type){
			vector<float> heur;
			int numSideCells = sqrt(cells.size());

			if(type == 0)
				for(Map::Cell &cell : cells)
					heur.push_back(145 * cells[dest].pos.getDistanceFrom(cell.pos));
			else if(type == 1)
				for(int i = 0; i < cells.size(); i++){
					int x = i % numSideCells, y = i / numSideCells;
					heur.push_back(2 * (numSideCells - 1) - (x + y));
				}

			return heur;
		}

		void PathfinderTest::testFindPath(){
			cells = vector<Map::Cell>{
				Map::Cell(Vector3::VEC_ZERO, Map::Cell::Type::LAND, vector<Map::Edge>{Map::Edge(0, 0, 0), Map::Edge(2, 0, 1), Map::Edge(4, 0, 2)}),
				Map::Cell(Vector3::VEC_ZERO, Map::Cell::Type::LAND, vector<Map::Edge>{Map::Edge(0, 1, 1), Map::Edge(1, 1, 2), Map::Edge(9, 1, 3), Map::Edge(13, 1, 4)}),
				Map::Cell(Vector3::VEC_ZERO, Map::Cell::Type::LAND, vector<Map::Edge>{Map::Edge(0, 2, 2), Map::Edge(2, 2, 1), Map::Edge(4, 2, 4), Map::Edge(5, 2, 5)}),
				Map::Cell(Vector3::VEC_ZERO, Map::Cell::Type::LAND, vector<Map::Edge>{Map::Edge(0, 3, 3), Map::Edge(1, 3, 6)}),
				Map::Cell(Vector3::VEC_ZERO, Map::Cell::Type::LAND, vector<Map::Edge>{Map::Edge(0, 4, 4), Map::Edge(1, 4, 1), Map::Edge(1, 4, 3), Map::Edge(2, 4, 5), Map::Edge(3, 4, 6)}),
				Map::Cell(Vector3::VEC_ZERO, Map::Cell::Type::LAND, vector<Map::Edge>{Map::Edge(0, 5, 5), Map::Edge(9, 5, 4), Map::Edge(2, 5, 6)}),
				Map::Cell(Vector3::VEC_ZERO, Map::Cell::Type::LAND, vector<Map::Edge>{Map::Edge(0, 6, 6)})
			};

			const u16 INF = u16(0 - 1);
			pathfinder->setImpassibleNodeVal(INF);

			int src = 0, dest = cells.size() - 1;
			vector<float> heur;
			vector<int> path = pathfinder->findPath(cells, heur, src, dest);
			CPPUNIT_ASSERT(path == vector<int>({0, 1, 2, 4, 3, 6}));

			int sumPathWeights = calcPathLength(path);
			CPPUNIT_ASSERT(sumPathWeights == 9);
		}

		void PathfinderTest::testFindBigPath(){
			cells = generateCellGraph(250);
			int numIterations = 1, src = 0, dest = cells.size() - 1;
			vector<float> heur;
			heur = generateHeuristics(cells, dest, 0);
			s64 sumTime = 0;

			for(int i = 0; i < numIterations; i++){
				s64 t0 = getTime();
				pathfinder->findPath(cells, heur, src, dest);
				s64 t1 = getTime();
				sumTime += t1 - t0;
			}

			int threshold = 100;
			double avg = (double)sumTime / numIterations, eps = .1;
			cout << "Total time: " << sumTime << endl;
			CPPUNIT_ASSERT(sumTime <= threshold);
		}

		void PathfinderTest::setUp(){
			pathfinder = Pathfinder::getSingleton();
		}

		void PathfinderTest::tearDown(){}
}
