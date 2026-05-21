#ifndef FREEZER_H
#define FREEZER_H

#include "vehicle.h"

namespace battleship{
	class Freezer : public Vehicle{
		public:
			Freezer(Player*, int, vb01::Vector3, vb01::Quaternion, Unit::State);
		private:
			void attack(Order);
			void build(Order);
	};
}

#endif
