#include "freezer.h"
#include "destructable.h"

namespace battleship{
	using namespace vb01;

	//TODO rename the class to a more generic name
	Freezer::Freezer(Player *player, int id, Vector3 pos, Quaternion rot, Unit::State state) : Vehicle(player, id, pos, rot, state){}

	void Freezer::attack(Order order){
		Vehicle::attack(order);

		Destructable *target = order.targets[0].unit->getDestructable();

		if(target && target->getFreezeStatus() == 100)
			removeOrder(0);
	}
}
