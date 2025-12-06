#ifndef CRYOGUN_H
#define CRYOGUN_H

#include "weapon.h"

#include <util.h>

#include <solUtil.h>

namespace battleship{
	class Unit;

	class CryoGun : public Weapon{
		public:
			CryoGun(Unit*, sol::table, int);
		private:
			vb01::s64 lastFreezeTime = 0;

			void updateTarget(GameObject*);
			inline bool canFreeze(){return vb01::getTime() - lastFreezeTime > rateOfFire;}
	};
}

#endif

