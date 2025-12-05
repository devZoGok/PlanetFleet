#include "cryoGun.h"
#include "destructable.h"
#include "unit.h"

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace gameBase;

	CryoGun::CryoGun(Unit *u, sol::table unitTable, int wid) : Weapon(u, unitTable, wid){}

	void CryoGun::updateTarget(GameObject *target){
		if(canFreeze()){
			Destructable *destructTarg = target->getDestructable();
			destructTarg->setFreezeStatus(destructTarg->getFreezeStatus() + 1);
			lastFreezeTime = getTime();
		}
	}
}

