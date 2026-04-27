#ifndef TORPEDO_H
#define TORPEDO_H

#include "projectile.h"

namespace battleship{
	class Torpedo : public Projectile{
		public:
			Torpedo(Unit*, int, vb01::Vector3, vb01::Quaternion);
			void update();
		private:
	};
}

#endif
