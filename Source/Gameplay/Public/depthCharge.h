#ifndef DEPTH_CHARGE_H
#define DEPTH_CHARGE_H

#include "projectile.h"

namespace battleship{
	class DepthCharge : public Projectile{
		public:
			DepthCharge(Unit*, int, vb01::Vector3, vb01::Quaternion);
			void update();
		private:
	};
}

#endif
