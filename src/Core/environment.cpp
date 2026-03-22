#include "environment.h"
#include "fxManager.h"
#include "player.h"
#include "unit.h"
#include "game.h"
#include "destructable.h"

#include <root.h>
#include <node.h>
#include <material.h>
#include <texture.h>
#include <particleEmitter.h>

namespace battleship{
	using namespace std;
	using namespace vb01;

	static Environment *environment = nullptr;

	Environment* Environment::getSingleton(){
		if(!environment)
			environment = new Environment();

		return environment;
	}

	//TODO use the pos argument from the fx
	void Environment::explode(FxManager::Fx *fx, Detonation det, Vector3 pos, int damage, float radius){
		FxManager::getSingleton()->addFx(fx);

		if(radius == 0) return;

		for(Player *pl : Game::getSingleton()->getPlayers()){
			for(Unit *un : pl->getUnits()){
				float distance = un->getPos().getDistanceFrom(pos);

				if(distance < radius){
					switch(det){
						case Detonation::EXPLOSION:
							un->getDestructable()->takeDamage(int(damage * (1.f - distance / radius)));
							break;
						case Detonation::EMP:
							un->setCondition(Unit::Condition::EM_JAMMED);
							break;
					}
				}
			}
		}
	}
}
