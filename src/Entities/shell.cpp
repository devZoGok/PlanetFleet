#include "shell.h"

#include <solUtil.h>

#include <cmath>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace gameBase;

	Shell::Shell(Unit *un, int id, Vector3 pos, Quaternion rot) : 
		Projectile(un, id, pos, rot), 
		initTime(getTime()),
		initDir(dirVec) {}

	void Shell::update(){
		float currTime = float(getTime() - initTime) / 1000;

		if(currTime == 0) return;

		const float G = generateView()["G"];
		float angle = initDir.getAngleBetween(Vector3(initDir.x, 0, initDir.z).norm()); 

		dirVec = (
			pos + 
			speed * currTime * cos(angle) * Vector3(initDir.x, 0, initDir.z).norm() + 
			(currTime * speed * sin(angle) - G * currTime * currTime) * Vector3::VEC_J -
			pos
		).norm();
		upVec = Quaternion(-PI / 2, leftVec) * dirVec;
		model->lookAt(dirVec, upVec);

		Projectile::update();

        if(!remove){
			checkSurfaceCollision(false);
			if(remove) return;
			checkUnitCollision();
		}
	}
}
