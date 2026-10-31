#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "map.h"

#include <vector>
#include <tuple>

#include <util.h>

namespace battleship{
	class Vehicle;

	class Pathfinder{
		public:
			static Pathfinder* getSingleton();
			std::vector<CellId> findPath(CellId, CellId, int = -1);
			inline vb01::u32 getImpassibleNodeVal(){return impassibleNodeVal;}
			inline void setImpassibleNodeVal(vb01::u32 val){this->impassibleNodeVal = val;}
		private:
			Pathfinder(){}
			std::vector<CellId> returnPath(CellId, int);
			inline float calcHeuristic(vb01::Vector3 cp, vb01::Vector3 dp){return 145 * dp.getDistanceFrom(cp);}

			vb01::u32 impassibleNodeVal;
			std::vector<vb01::u32> **distances = nullptr;
			std::vector<std::vector<CellId>> **paths = nullptr;
			std::vector<bool> **cellsByCheck = nullptr;
			std::vector<bool> **possibleMinCellIncludes = nullptr;
	};
}

#endif
