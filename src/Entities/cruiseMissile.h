#ifndef CRUISE_MISSILE_H
#define CRUISE_MISSILE_H

#include "projectile.h"

namespace battleship{
	class CruiseMissile : public Projectile{
		public:
			CruiseMissile(Unit*, int, vb01::Vector3, vb01::Vector3, vb01::Quaternion);
			~CruiseMissile();
			void update();
		private:
			enum class FlightStage{ASCENT, CRUISE, DESCENT};
			FlightStage flightStage;
			vb01::Vector3 targetPoint;

			void pitch(float, vb01::Vector3);
			void cruise();
	};
}

#endif
