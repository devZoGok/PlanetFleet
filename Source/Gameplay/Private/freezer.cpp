#include "freezer.h"
#include "weapon.h"
#include "structure.h"
#include "destructable.h"

#include <vector>

namespace battleship{
	using namespace vb01;
	using namespace std;

	//TODO rename the class to a more generic name
	Freezer::Freezer(Player *player, int id, Vector3 pos, Quaternion rot, Unit::State state) : Vehicle(player, id, pos, rot, state){}

	void Freezer::attack(Order order){
		Vehicle::attack(order);

		Destructable *target = order.targets[0].unit->getDestructable();

		if(target && target->getFreezeStatus() >= 100)
			removeOrder(0);
	}

	void Freezer::build(Order order){
		vector<Weapon*> weapons = getWeaponsByType((int)Weapon::Type::FREEZER);

		if(weapons.empty()) return;

		Unit *targ = (Unit*)order.targets[0].unit;

		for(Weapon *w : weapons)
			w->trackTarget(targ->getPos());

		Vehicle::build(order);

		if(targ->getUnitClass() == UnitClass::ICE_SHEET && ((Structure*)targ)->isComplete())
			removeOrder(0);
	}
}
