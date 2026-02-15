#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "map.h"

#include <vector>

#include <util.h>

namespace battleship{
	class Vehicle;

	class Pathfinder{
		public:
			static Pathfinder* getSingleton();
			std::vector<float> calcHeuristics(std::vector<Map::Cell>&, int);
			std::vector<int> findPath(std::vector<Map::Cell>&, std::vector<float>&, int, int, Vehicle* = nullptr);
			inline vb01::u32 getImpassibleNodeVal(){return impassibleNodeVal;}
			inline void setImpassibleNodeVal(vb01::u32 val){this->impassibleNodeVal = val;}
		private:
			Pathfinder(){}

			vb01::u32 impassibleNodeVal;
	};
}

#endif
