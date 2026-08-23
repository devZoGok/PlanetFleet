#ifndef RESOURCE_ROVER_H
#define RESOURCE_ROVER_H

#include "vehicle.h"

#include <util.h>

namespace battleship{
	class Structure;
	class Extractor;

	class ResourceRover : public Vehicle{
		public:
			ResourceRover(Player*, int, vb01::Vector3, vb01::Quaternion, Unit::State = Unit::State::STAND_GROUND);
			~ResourceRover();
			void update();
			inline int getLoad(int id){return cargo[id];}
			inline int calcTotalLoad(){return cargo[0] + cargo[1] + cargo[2];}
			inline int getCapacity(){return capacity;}
			// Checks if enough time has passed since the last load to keep with the rover's loading rate and that it hasn't loaded long enough to reach capacity
			inline bool canLoad(){return vb01::getTime() - lastLoadTime > loadRate && calcTotalLoad() < capacity;}
			// Checks if enough time has passed since the last load to keep with the rover's loading rate
			inline bool canUnload(int id){return vb01::getTime() - lastLoadTime > loadRate && cargo[id] > 0;}
		private:
			void initProperties();
			void collectRefineds(Order);
			void loadResources(Structure*, bool);
			void handleResources(Order); // Overrides the handleResources function from unit class
			Unit* getClosestUnit(std::vector<Structure*>);

			vb01::s64 lastLoadTime = 0;
			int cargo[3]{0, 0, 0}, capacity, loadRate, loadSpeed;
			Extractor *nearestExtractor = nullptr; 
			Unit *nearestRefinery = nullptr;
			vb01::Node *loadBackground = nullptr, *loadForeground = nullptr;
	};
}

#endif
