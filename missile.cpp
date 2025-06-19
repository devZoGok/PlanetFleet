#include "missile.h"

namespace battleship{
	using namespace vb01;

	Missile::Missile(Unit *un, int id, Vector3 tp, Vector3 pos, Quaternion rot) : 
		Projectile(un, id, pos, rot),
		targetPos(tp){}

	void Missile::update(){
		Vector3 targDir = (targetPos - pos).norm();
		float angle = targDir.getAngleBetween(dirVec);
		float ra = (rotAngle < angle ? rotAngle : angle);
		orientAt(Quaternion(ra, dirVec.cross(targDir)) * rot);

		Projectile::update();
		checkCollision();
	}
}
