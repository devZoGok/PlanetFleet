#ifndef POINT_DEFENSE_H
#define POINT_DEFENSE_H

#include "structure.h"

namespace battleship{
	class PointDefense : public Structure{
		public:
			PointDefense(Player*, int, vb01::Vector3, vb01::Quaternion, int, Unit::State);
		private:
			void attack(Order);
	};
}

#endif
