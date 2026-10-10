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

		vector<CellId> Pathfinder::findPath(CellId source, CellId dest, int vehicleType, CellId graphOrigin, CellId graphEnd, int cellScale){
			CellId numCells = Map::getSingleton()->getNumCells();

			const u16 numX = (const u16)get<0>(numCells), numY = (const u16)get<1>(numCells), numZ = (const u16)get<2>(numCells);
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

			paths[get<0>(source)][get<2>(source)][get<1>(source)].push_back(source);
			distances[get<0>(source)][get<2>(source)][get<1>(source)] = 0;

			Map::Cell destCell = map->getCell(dest);

			while(!cellsByCheck[get<0>(dest)][get<2>(dest)][get<1>(dest)]){
				int minCellId = 0;
				CellId vertStrich = possibleMinCells[minCellId].first;
				Map::Cell vertStrichCell = map->getCell(vertStrich, cellScale, cellScale, cellScale);

				for(int i = 0; i < possibleMinCells.size(); i++){
					CellId pmc1 = possibleMinCells[i].first;
					CellId pmc2 = possibleMinCells[minCellId].first;

					float heurPmc1 = calcHeuristic(possibleMinCells[i].second.pos, destCell.pos);
					float heurPmc2 = calcHeuristic(possibleMinCells[minCellId].second.pos, destCell.pos);

					float sum1 = distances[get<0>(pmc1)][get<2>(pmc1)][get<1>(pmc1)] + heurPmc1; 
					float sum2 = distances[get<0>(pmc2)][get<2>(pmc2)][get<1>(pmc2)] + heurPmc2; 

					if(sum1 < sum2 || (sum1 == sum2 && heurPmc1 < heurPmc2)){
						minCellId = i;
						vertStrich = possibleMinCells[i].first;
						vertStrichCell = map->getCell(vertStrich, cellScale, cellScale, cellScale);
					}
				}

				possibleMinCellIncludes[get<0>(possibleMinCells[minCellId].first)][get<2>(possibleMinCells[minCellId].first)][get<1>(possibleMinCells[minCellId].first)] = false;
				possibleMinCells.erase(possibleMinCells.begin() + minCellId);

				cellsByCheck[get<0>(vertStrich)][get<2>(vertStrich)][get<1>(vertStrich)] = true;

				int numEdges = vertStrichCell.edges.size();

				for(int i = 0; i < numEdges; i++){
					CellId enId = vertStrichCell.edges[i].destCellId;
					Map::Cell edgeNode = map->getCell(enId, cellScale, cellScale, cellScale);

					if(cellsByCheck[get<0>(enId)][get<2>(enId)][get<1>(enId)]) continue;

					if(!possibleMinCellIncludes[get<0>(enId)][get<2>(enId)][get<1>(enId)]){
						possibleMinCellIncludes[get<0>(enId)][get<2>(enId)][get<1>(enId)] = true;
						possibleMinCells.push_back(make_pair(enId, edgeNode));
					}

					UnitType ut = (UnitType)vehicleType;
					Map::Cell::Type ct = edgeNode.type;
					bool ship = (ut == UnitType::UNDERWATER || ut == UnitType::SEA_LEVEL);
					int weightMult = 1;

					if((ut == UnitType::LAND && ct == Map::Cell::WATER) || (ship && ct == Map::Cell::LAND))
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

					if(distances[get<0>(vertStrich)][get<2>(vertStrich)][get<1>(vertStrich)] + vertStrichCell.edges[i].weight * weightMult < distances[get<0>(enId)][get<2>(enId)][get<1>(enId)]){
						distances[get<0>(enId)][get<2>(enId)][get<1>(enId)] = distances[get<0>(vertStrich)][get<2>(vertStrich)][get<1>(vertStrich)] + vertStrichCell.edges[i].weight * weightMult;
						paths[get<0>(enId)][get<2>(enId)][get<1>(enId)] = paths[get<0>(vertStrich)][get<2>(vertStrich)][get<1>(vertStrich)];
						paths[get<0>(enId)][get<2>(enId)][get<1>(enId)].push_back(enId);
					}
				}
			}

			return paths[get<0>(dest)][get<2>(dest)][get<1>(dest)];
		}
}
