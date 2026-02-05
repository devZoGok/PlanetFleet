#include "pointDefense.h"

#include <string>

namespace battleship{
	using namespace vb01;
	using namespace std;
	using namespace gameBase;

	PointDefense::PointDefense(Player *player, int id, Vector3 pos, Quaternion rot, int buildStatus, Unit::State state) : Structure(player, id, pos, rot, buildStatus, state){}

	void PointDefense::attack(Order order){
		if(!isComplete()) return;

		Unit::attack(order);
	}
}
