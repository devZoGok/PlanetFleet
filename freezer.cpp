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

	/// @brief Freeze ice sheets which constitutes building/ reapairing them
	/// @param order Must be a structure
	void Freezer::build(Order order){
		// Get all the freezer weapons and if there are none then return
		vector<Weapon*> weapons = getWeaponsByType((int)Weapon::Type::FREEZER);

		if(weapons.empty()) return;

		Unit *targ = (Unit*)order.targets[0].unit;

		// Have all the freezer weapons on the freezer get the position of the target, 
		for(Weapon *w : weapons)
			w->trackTarget(targ->getPos());

		// Build the structure included in the order, the freezer is a vehicle so it will be checked to see if it is close enough to the structure
		Vehicle::build(order);

		// If the target is an ice sheet and the target structure is complete being built then remove the order
		if(targ->getUnitClass() == UnitClass::ICE_SHEET && ((Structure*)targ)->isComplete())
			removeOrder(0);
	}
}
