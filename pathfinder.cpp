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

		/*
		int Pathfinder::clampDestToSourceRegion(int source, int dest){
			Map *map = Map::getSingleton();
			vector<Map::Cell> &cells = map->getCells();
			pair<Map::Cell::Type, vector<Map::Cell*>> srcRegion;

			for(int i = 0; i < map->getNumRegions(); i++){
				pair<Map::Cell::Type, vector<Map::Cell*>> region = map->getRegion(i);

				if(find(region.second.begin(), region.second.end(), &cells[source]) != region.second.end()){
					srcRegion = map->getRegion(i);
					break;
				}
			}

			if(find(srcRegion.second.begin(), srcRegion.second.end(), &cells[dest]) == srcRegion.second.end()){
				int minDistId = 0;

				for(int i = 0; i < srcRegion.second.size(); i++){
					float currDist = cells[dest].pos.getDistanceFrom(srcRegion.second[i]->pos);
					float minDist = cells[dest].pos.getDistanceFrom(srcRegion.second[minDistId]->pos);

					if(currDist < minDist) minDistId = i;
				}

				dest = map->getCellId(srcRegion.second[minDistId]->pos);
			}

			return dest;
		}
		*/

		float Pathfinder::calcHeuristic(CellId cell, CellId dest){
			Map *map = Map::getSingleton();
			return 145 * map->getCell(dest).pos.getDistanceFrom(map->getCell(cell).pos);
		}

		vector<CellId> Pathfinder::findPath(CellId source, CellId dest, int vehicleType, CellId graphOrigin, CellId graphEnd, int cellScale){
			CellId numCells = Map::getSingleton()->getNumCells();

			const u16 numX = (const u16)get<0>(numCells), numY = (const u16)get<1>(numCells), numZ = (const u16)get<2>(numCells);
			vector<u32> distances[numX][numZ];
			vector<vector<CellId>> paths[numX][numZ];
			vector<bool> cellsByCheck[numX][numZ];

			vector<bool> possibleMinCellIncludes[numX][numZ];
			vector<CellId> possibleMinCells = vector<CellId>{source};

			for(int x = 0; x < numX; x++){
				for(int z = 0; z < numZ; z++){
					for(int y = 0; y < numY; y++){
						paths[x][z].push_back(vector<CellId>{});
						cellsByCheck[x][z].push_back(false);
						distances[x][z].push_back(impassibleNodeVal);
						possibleMinCellIncludes[x][z].push_back(false);
					}
				}
			}

			paths[get<0>(source)][get<2>(source)][get<1>(source)].push_back(source);
			distances[get<0>(source)][get<2>(source)][get<1>(source)] = 0;

			CellId lastVertStrich = make_tuple(-1, -1, -1);
			Map *map = Map::getSingleton();

			while(!cellsByCheck[get<0>(dest)][get<2>(dest)][get<1>(dest)]){
				int minCellId = 0;
				CellId vertStrich = possibleMinCells[minCellId];

				for(int i = 0; i < possibleMinCells.size(); i++){
					CellId pmc1 = possibleMinCells[i];
					CellId pmc2 = possibleMinCells[minCellId];

					float heurPmc1 = calcHeuristic(possibleMinCells[i], dest);
					float heurPmc2 = calcHeuristic(possibleMinCells[minCellId], dest);

					float sum1 = distances[get<0>(pmc1)][get<2>(pmc1)][get<1>(pmc1)] + heurPmc1; 
					float sum2 = distances[get<0>(pmc2)][get<2>(pmc2)][get<1>(pmc2)] + heurPmc2; 

					if(sum1 < sum2 || (sum1 == sum2 && heurPmc1 < calcHeuristic(vertStrich, dest))){
						minCellId = i;
						vertStrich = possibleMinCells[i];
					}
				}

				if(distances[get<0>(vertStrich)][get<2>(vertStrich)][get<1>(vertStrich)] == impassibleNodeVal)
					return paths[get<0>(lastVertStrich)][get<2>(lastVertStrich)][get<1>(lastVertStrich)];

				possibleMinCellIncludes[get<0>(possibleMinCells[minCellId])][get<2>(possibleMinCells[minCellId])][get<1>(possibleMinCells[minCellId])] = false;
				possibleMinCells.erase(possibleMinCells.begin() + minCellId);

				cellsByCheck[get<0>(vertStrich)][get<2>(vertStrich)][get<1>(vertStrich)] = true;

				Map::Cell vertStrichCell = map->getCell(vertStrich, cellScale, cellScale, cellScale);
				int numEdges = vertStrichCell.edges.size();

				for(int i = 0; i < numEdges; i++){
					CellId enId = vertStrichCell.edges[i].destCellId;
					Map::Cell edgeNode = map->getCell(enId, cellScale, cellScale, cellScale);

					if(cellsByCheck[get<0>(enId)][get<2>(enId)][get<1>(enId)]) continue;

					if(!possibleMinCellIncludes[get<0>(enId)][get<2>(enId)][get<1>(enId)]){
						possibleMinCellIncludes[get<0>(enId)][get<2>(enId)][get<1>(enId)] = true;
						possibleMinCells.push_back(enId);
					}

					UnitType ut = (UnitType)vehicleType;
					Map::Cell::Type ct = edgeNode.type;
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

					if(distances[get<0>(vertStrich)][get<2>(vertStrich)][get<1>(vertStrich)] + vertStrichCell.edges[i].weight < distances[get<0>(enId)][get<2>(enId)][get<1>(enId)]){
						distances[get<0>(enId)][get<2>(enId)][get<1>(enId)] = distances[get<0>(vertStrich)][get<2>(vertStrich)][get<1>(vertStrich)] + vertStrichCell.edges[i].weight;
						paths[get<0>(enId)][get<2>(enId)][get<1>(enId)] = paths[get<0>(vertStrich)][get<2>(vertStrich)][get<1>(vertStrich)];
						paths[get<0>(enId)][get<2>(enId)][get<1>(enId)].push_back(enId);
					}
				}

				lastVertStrich = vertStrich;
			}

			return paths[get<0>(dest)][get<2>(dest)][get<1>(dest)];
		}
}
