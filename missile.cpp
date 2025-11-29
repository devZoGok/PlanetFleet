#include "missile.h"
#include "unit.h"

namespace battleship{
	using namespace vb01;

	Missile::Missile(Unit *un, int id, Vector3 tp, Vector3 pos, Quaternion rot) : 
		Projectile(un, id, pos, rot),
		Destructable(un->getPlayer(), id, GameObject::Type::PROJECTILE, pos, rot), 
		targetPos(tp){}

	void Missile::update(){
		Vector3 targDir = (targetPos - Projectile::pos).norm();
		float angle = targDir.getAngleBetween(Projectile::dirVec);
		float ra = (rotAngle < angle ? rotAngle : angle);
		Projectile::orientAt(Quaternion(ra, Projectile::dirVec.cross(targDir)) * Projectile::rot);

		Projectile::update();
		checkCollision();
	}
}
