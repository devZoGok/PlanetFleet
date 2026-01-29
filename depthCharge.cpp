#include "depthCharge.h"

namespace battleship{
	using namespace vb01;

    DepthCharge::DepthCharge(Unit *un, int id, Vector3 pos, Quaternion rot) : Projectile(un, id, pos, rot){}

	void DepthCharge::update(){
		Projectile::update();

        if(!remove) checkUnitCollision();
	}
}
