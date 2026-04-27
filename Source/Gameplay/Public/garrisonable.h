#ifndef GARRISONABLE_H
#define GARRISONABLE_H

#include <vector>

#include "unit.h"

namespace vb01{
	class Node;
}

namespace battleship{
	class Vehicle;

	class Garrisonable{
		public:
			Garrisonable(){}
			~Garrisonable(){}
			void update();
			void ejectVehicle(int);
		protected:
			int capacity;
			std::vector<Vehicle*> garrisonedVehicles;
			std::vector<vb01::Node*> garrisonSlotForegrounds, garrisonSlotBackgrounds;

			void prepareGarrisonSlots();
	};
}

#endif
