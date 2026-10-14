#include <algorithm>
#include <map>

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

		vector<CellId> Pathfinder::findPath(CellId source, CellId dest, int vehicleType){
			CellId numCells = Map::getSingleton()->getNumCells();
			int cellScale = 1;

			const u16 numX = numCells.x, numY = numCells.y, numZ = numCells.z;
			vector<u32> distances[numX][numZ];
			vector<vector<CellId>> paths[numX][numZ];
			vector<bool> cellsByCheck[numX][numZ];

			vector<bool> possibleMinCellIncludes[numX][numZ];
			Map *map = Map::getSingleton();
			vector<pair<CellId, Map::Cell>> possibleMinCells = vector<pair<CellId, Map::Cell>>{make_pair(source, map->getCell(source))};

			for(int x = 0; x < numX; x++){
				for(int z = 0; z < numZ; z++){
					int depth = map->getCell(x, 0, z).numUnderWaterCells + 1;

					for(int y = 0; y < depth; y++){
						paths[x][z].push_back(vector<CellId>{});
						cellsByCheck[x][z].push_back(false);
						distances[x][z].push_back(impassibleNodeVal);
						possibleMinCellIncludes[x][z].push_back(false);
					}
				}
			}

			paths[source.x][source.z][source.y].push_back(source);
			distances[source.x][source.z][source.y] = 0;

			Map::Cell destCell = map->getCell(dest);
			UnitType unitType = (UnitType)vehicleType;
			bool ship = (unitType == UnitType::UNDERWATER || unitType == UnitType::SEA_LEVEL);

			while(!cellsByCheck[dest.x][dest.z][dest.y]){
				int minCellId = 0;
				CellId vertStrich = possibleMinCells[minCellId].first;
				Map::Cell vertStrichCell = map->getCell(vertStrich, cellScale, cellScale, cellScale);

				for(int i = 0; i < possibleMinCells.size(); i++){
					CellId pmc1 = possibleMinCells[i].first;
					CellId pmc2 = possibleMinCells[minCellId].first;

					float heurPmc1 = calcHeuristic(possibleMinCells[i].second.pos, destCell.pos);
					float heurPmc2 = calcHeuristic(possibleMinCells[minCellId].second.pos, destCell.pos);

					float sum1 = distances[pmc1.x][pmc1.z][pmc1.y] + heurPmc1; 
					float sum2 = distances[pmc2.x][pmc2.z][pmc2.y] + heurPmc2; 

					if(sum1 < sum2 || (sum1 == sum2 && heurPmc1 < heurPmc2)){
						minCellId = i;
						vertStrich = possibleMinCells[i].first;
						vertStrichCell = map->getCell(vertStrich, cellScale, cellScale, cellScale);
					}
				}

				if((vertStrichCell.type == Map::Cell::WATER && unitType == UnitType::LAND) || (vertStrichCell.type == Map::Cell::LAND && ship))
					return paths[vertStrich.x][vertStrich.z][vertStrich.y];

				possibleMinCellIncludes[possibleMinCells[minCellId].first.x][possibleMinCells[minCellId].first.z][possibleMinCells[minCellId].first.y] = false;
				possibleMinCells.erase(possibleMinCells.begin() + minCellId);

				cellsByCheck[vertStrich.x][vertStrich.z][vertStrich.y] = true;

				int numEdges = vertStrichCell.edges.size();

				for(int i = 0; i < numEdges; i++){
					CellId enId = vertStrichCell.edges[i].destCellId;
					Map::Cell edgeNode = map->getCell(enId, cellScale, cellScale, cellScale);

					if(cellsByCheck[enId.x][enId.z][enId.y]) continue;

					if(!possibleMinCellIncludes[enId.x][enId.z][enId.y]){
						possibleMinCellIncludes[enId.x][enId.z][enId.y] = true;
						possibleMinCells.push_back(make_pair(enId, edgeNode));
					}

					Map::Cell::Type ct = edgeNode.type;
					int weightMult = 1;

					if((unitType == UnitType::LAND && ct == Map::Cell::WATER) || (ship && ct == Map::Cell::LAND))
						weightMult = 1000;
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

					if(distances[vertStrich.x][vertStrich.z][vertStrich.y] + vertStrichCell.edges[i].weight * weightMult < distances[enId.x][enId.z][enId.y]){
						distances[enId.x][enId.z][enId.y] = distances[vertStrich.x][vertStrich.z][vertStrich.y] + vertStrichCell.edges[i].weight * weightMult;
						paths[enId.x][enId.z][enId.y] = paths[vertStrich.x][vertStrich.z][vertStrich.y];
						paths[enId.x][enId.z][enId.y].push_back(enId);
					}
				}
			}

			return paths[dest.x][dest.z][dest.y];
		}
}
