#ifndef SUBMARINE_H
#define SUBMARINE_H

#include "vehicle.h"

namespace battleship{
	class Submarine : public Vehicle{
		public:
			Submarine(Player*, int, vb01::Vector3, vb01::Quaternion, Unit::State);
		private:
			bool validateLaunchOrder();
	};
}

#endif
