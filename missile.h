#ifndef MISSILE_H
#define MISSILE_H

#include "projectile.h"

namespace battleship{
	class Missile : public Projectile{
		public:
			Missile(Unit*, int, vb01::Vector3, vb01::Vector3, vb01::Quaternion);
			~Missile(){}
			void update();
		private:
			vb01::Vector3 targetPos;
	};
}

#endif
