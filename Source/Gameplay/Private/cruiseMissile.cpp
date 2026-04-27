#include "cruiseMissile.h"
#include "destructable.h"
#include "player.h"
#include "unit.h"
#include "map.h"
#include "game.h"

#include <util.h>

#include <cmath>

namespace battleship{
	using namespace std;
	using namespace vb01;

	CruiseMissile::CruiseMissile(Unit *unit, int id, Vector3 tp, Vector3 pos, Quaternion rot) : 
		Projectile(unit, id, pos, rot), 
		targetPoint(tp), 
		flightStage(FlightStage::ASCENT)
	{
		destructable = new Destructable(this);

		Vector3 unitDir = unit->getDirVec(); 
		Vector3 leftDir = unit->getLeftVec();
		Vector3 targDir = (Vector3(targetPoint.x, pos.y, targetPoint.z) - pos).norm();
		
		bool left = (leftDir.getAngleBetween(targDir) < PI / 2);
		float angle = unitDir.getAngleBetween(targDir) * (left ? 1 : -1);
		orientAt(Quaternion(angle, Vector3::VEC_J) * rot);
	}

	CruiseMissile::~CruiseMissile(){delete destructable;}

	void CruiseMissile::pitch(float rotAngle, Vector3 compVec){
		float minHeight = 20;
		float angleToCompVec = Projectile::dirVec.getAngleBetween(compVec);
		float angle = (angleToCompVec > rotAngle ? rotAngle : angleToCompVec);

		if(flightStage == FlightStage::ASCENT && Projectile::pos.y - initPos.y > minHeight){
			Projectile::orientAt(Quaternion(angle, Projectile::leftVec) * Projectile::rot);
			if(Projectile::dirVec.y <= .001) flightStage = FlightStage::CRUISE;
		}
		else if(flightStage == FlightStage::DESCENT){
			Vector3 targDir = (Vector3(targetPoint.x, initPos.y, targetPoint.z) - initPos).norm();

			if(Projectile::dirVec.getAngleBetween(targDir) < PI / 2)
				Projectile::orientAt(Quaternion(angle, Projectile::leftVec) * Projectile::rot);
		}
	}

	void CruiseMissile::cruise(){
		float minDist = 6;
		float initDist = Vector3(targetPoint.x, initPos.y, targetPoint.z).getDistanceFrom(initPos);
		
		if(Projectile::pos.getDistanceFrom(Vector3(targetPoint.x, Projectile::pos.y, targetPoint.z)) < minDist)
			flightStage = FlightStage::DESCENT;
	}

	void CruiseMissile::update(){
		Projectile::update();
		destructable->update();

		switch(flightStage){
			case FlightStage::ASCENT:
				pitch(rotAngle, Vector3(Projectile::dirVec.x, 0, Projectile::dirVec.z).norm());
				break;
			case FlightStage::DESCENT:
				pitch(rotAngle, -Vector3::VEC_J);
				break;
			case FlightStage::CRUISE:
				cruise();
				break;
		}

		if(flightStage == FlightStage::DESCENT && !remove){
			checkSurfaceCollision(false);
			if(remove) return;
			checkUnitCollision();
		}
	}
}
