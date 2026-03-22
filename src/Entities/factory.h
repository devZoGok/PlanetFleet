#ifndef FACTORY_H
#define FACTORY_H

#include "structure.h"

#include <vector>
#include <util.h>

namespace vb01{
	class Node;
}

namespace battleship{
	class Player;

	class Factory : public Structure{
		public:
			Factory(Player*, int, vb01::Vector3, vb01::Quaternion, int = 0, Unit::State = Unit::State::STAND_GROUND);
			~Factory(){}
			void update();
			int getNumQueueUnitsById(int);
			void appendToQueue(int);
			inline std::vector<int> getQueue(){return unitQueue;}
		private:
			std::vector<int> unitQueue;
			int maxLenQueue, trainingStatus = 0;
			vb01::s64 lastTrainTime = 0;

			void initProperties();
			void train();
	};
}

#endif
