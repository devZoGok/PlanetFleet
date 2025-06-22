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

		vector<int> Pathfinder::findPath(vector<Map::Cell> &cells, vector<float> &heuristics, int source, int dest, Vehicle *vehicle){
			const int size = cells.size();
			u32 *distances = new u32[size];
			vector<int> *paths = new vector<int>[size];
			paths[source].push_back(source);

			vector<pair<int, bool>> cellsByCheck;

			vector<bool> cellChecked;

			for(int i = 0; i < size; i++){
				cellsByCheck.push_back(pair(i, false));
				distances[i] = (i == source ? 0 : impassibleNodeVal);
				cellChecked.push_back(false);
			}

			bool useHeur = !heuristics.empty();
			vector<int> possibleMinCells = vector<int>{source};
			cellChecked[source] = true;

			while(!cellsByCheck[dest].second){
				int vertStrich = possibleMinCells[0], vsId = 0;

				for(int i = 0; i < possibleMinCells.size(); i++){
					float sum1 = distances[possibleMinCells[i]] + (useHeur ? heuristics[possibleMinCells[i]] : 0); 
					float sum2 = distances[possibleMinCells[vsId]] + (useHeur ? heuristics[possibleMinCells[vsId]] : 0); 

					if(sum1 < sum2 || (useHeur && sum1 == sum2 && heuristics[i] < heuristics[vertStrich])){
						vertStrich = possibleMinCells[i];
						vsId = i;
					}
				}

				cellChecked[possibleMinCells[vsId]] = false;
				possibleMinCells.erase(possibleMinCells.begin() + vsId);

				cellsByCheck[vertStrich].second = true;

				int numEdges = cells[vertStrich].edges.size();

				for(int i = 0; i < numEdges; i++){
					int edgeNode = cells[vertStrich].edges[i].destCellId;

					if(cellsByCheck[edgeNode].second) continue;

					if(!cellChecked[edgeNode]){
						cellChecked[edgeNode] = true;
						possibleMinCells.push_back(edgeNode);
					}

					bool canMoveToStrichCell = true;

					if(vehicle){
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

					if(distances[vertStrich] + cells[vertStrich].edges[i].weight < distances[edgeNode]){
						distances[edgeNode] = distances[vertStrich] + cells[vertStrich].edges[i].weight;
						paths[edgeNode] = paths[vertStrich];
						paths[edgeNode].push_back(edgeNode);
					}
				}
			}

			vector<int> path = paths[dest];

			delete[] paths;
			delete[] distances;

			return path;
		}
}
