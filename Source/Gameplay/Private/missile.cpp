#include "missile.h"
#include "unit.h"
#include "destructable.h"

namespace battleship{
	using namespace vb01;

	Missile::Missile(Unit *un, int id, Vector3 tp, Vector3 pos, Quaternion rot) : 
		Projectile(un, id, pos, rot),
		targetPos(tp)
	{
		destructable = new Destructable(this);
	}

	Missile::~Missile(){delete destructable;}

	void Missile::update(){
		Vector3 targDir = (targetPos - Projectile::pos).norm();
		float angle = targDir.getAngleBetween(Projectile::dirVec);
		float ra = (rotAngle < angle ? rotAngle : angle);
		orientAt(Quaternion(ra, Projectile::dirVec.cross(targDir)) * Projectile::rot);

		Projectile::update();
		destructable->update();

        if(!remove){
			checkSurfaceCollision(false);
			if(remove) return;
			checkUnitCollision();
		}
	}
}
