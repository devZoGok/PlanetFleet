#include "torpedo.h"

namespace battleship{
	using namespace vb01;

    Torpedo::Torpedo(Unit *un, int id, Vector3 pos, Quaternion rot) : Projectile(un, id, pos, rot){}

	void Torpedo::update(){
		Projectile::update();

        if(!remove){
			checkSurfaceCollision(true);
			if(remove) return;
			checkUnitCollision();
		}
	}
}
