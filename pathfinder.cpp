#include <algorithm>

#include "pathfinder.h"
#include "vehicle.h"

namespace battleship{
		using namespace std;
		using namespace vb01;

		static Pathfinder *pathfinder = nullptr;

		Pathfinder* Pathfinder::getSingleton(){
			if(!pathfinder)
				pathfinder = new Pathfinder();

			return pathfinder;
		}

		vector<float> Pathfinder::calcHeuristics(vector<Map::Cell> &cells, int dest){
			vector<float> heuristics;

			for(Map::Cell &cell : cells)
				heuristics.push_back(145 * (cells[dest].pos.getDistanceFrom(cell.pos)));

			return heuristics;
		}

		vector<int> Pathfinder::findPath(vector<Map::Cell> &cells, vector<float> &heuristics, int source, int dest, int vehicleType){
			if(heuristics.empty() || (heuristics.size() == 1 && heuristics[0] == 0.0))
				heuristics = calcHeuristics(cells, dest);

			bool useHeur = !heuristics.empty();

			const int size = cells.size();
			u32 *distances = new u32[size];
			vector<int> *paths = new vector<int>[size];
			vector<pair<int, bool>> cellsByCheck;
			vector<bool> posMinCellChecked;
			vector<int> possibleMinCells = vector<int>{source};

			for(int i = 0; i < size; i++){
				cellsByCheck.push_back(pair(i, false));
				distances[i] = impassibleNodeVal;
				posMinCellChecked.push_back(false);
			}

			paths[source].push_back(source);
			distances[source] = 0;
			posMinCellChecked[source] = true;

			int lastVertStrich = -1;

			while(!cellsByCheck[dest].second){
				int posMinCellId = 0, vertStrich = possibleMinCells[posMinCellId];

				for(int i = 0; i < possibleMinCells.size(); i++){
					float sum1 = distances[possibleMinCells[i]] + (useHeur ? heuristics[possibleMinCells[i]] : 0); 
					float sum2 = distances[possibleMinCells[posMinCellId]] + (useHeur ? heuristics[possibleMinCells[posMinCellId]] : 0); 

					if(sum1 < sum2 || (useHeur && sum1 == sum2 && heuristics[i] < heuristics[vertStrich])){
						posMinCellId = i;
						vertStrich = possibleMinCells[i];
					}
				}

				if(distances[vertStrich] == impassibleNodeVal){
					vector<int> path = paths[lastVertStrich];

					delete[] paths;
					delete[] distances;

					return path;
				}

				posMinCellChecked[possibleMinCells[posMinCellId]] = false;
				possibleMinCells.erase(possibleMinCells.begin() + posMinCellId);

				cellsByCheck[vertStrich].second = true;

				int numEdges = cells[vertStrich].edges.size();

				for(int i = 0; i < numEdges; i++){
					int edgeNode = cells[vertStrich].edges[i].destCellId;

					if(cellsByCheck[edgeNode].second) continue;

					if(!posMinCellChecked[edgeNode]){
						posMinCellChecked[edgeNode] = true;
						possibleMinCells.push_back(edgeNode);
					}

					UnitType ut = (UnitType)vehicleType;
					Map::Cell::Type ct = cells[edgeNode].type;
					bool ship = (ut == UnitType::UNDERWATER || ut == UnitType::SEA_LEVEL);

					if((ut == UnitType::LAND && ct == Map::Cell::WATER) || (ship && ct == Map::Cell::LAND))
						continue;
					/*
					if(vehicleType != -1){
						UnitType unitType = vehicle->getType();
						bool ship = (unitType == UnitType::UNDERWATER || unitType == UnitType::SEA_LEVEL);
						Unit *blockingUnit = cells[vertStrich].blockedBy;
						bool diffBlockingUnit = (blockingUnit && blockingUnit != vehicle);
						bool throughBlockedCell = (source != vertStrich);

						if(
								(unitType == UnitType::LAND && ((diffBlockingUnit && throughBlockedCell) || cells[vertStrich].type != Map::Cell::LAND)) ||
								(
									ship &&
									(
										cells[vertStrich].type != Map::Cell::WATER ||
										(
											diffBlockingUnit && 
											throughBlockedCell && 
											blockingUnit->getUnitClass() == UnitClass::ICE_SHEET && 
											vehicle->getUnitClass() != UnitClass::ICEBREAKER
										)
									)
								)
						)
						{
							continue;
						}
					}
					*/

					if(distances[vertStrich] + cells[vertStrich].edges[i].weight < distances[edgeNode]){
						distances[edgeNode] = distances[vertStrich] + cells[vertStrich].edges[i].weight;
						paths[edgeNode] = paths[vertStrich];
						paths[edgeNode].push_back(edgeNode);
					}
				}

				lastVertStrich = vertStrich;
			}

			vector<int> path = paths[dest];

			delete[] paths;
			delete[] distances;

			return path;
		}
}
