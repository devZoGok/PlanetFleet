#include "gameObjectFactory.h"
#include "player.h"
#include "factory.h"
#include "engineer.h"
#include "resourceRover.h"
#include "projectile.h"
#include "resourceDeposit.h"
#include "pointDefense.h"
#include "extractor.h"
#include "researchStruct.h"
#include "missile.h"
#include "cruiseMissile.h"
#include "shell.h"
#include "defConfigs.h"

#include <solUtil.h>

#include <vector>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace gameBase;

	Unit* GameObjectFactory::createUnit(Player *player, int id, Vector3 pos, Quaternion rot, int buildStatus){
		sol::state_view SOL_LUA_VIEW = generateView();
		int unitClass = SOL_LUA_VIEW["units"][id + 1]["unitClass"];

		switch((UnitClass)unitClass){
			case UnitClass::FORT:
			case UnitClass::LAND_FACTORY:
			case UnitClass::NAVAL_FACTORY:
				return new Factory(player, id, pos, rot, buildStatus, Unit::State::STAND_GROUND);
			case UnitClass::POINT_DEFENSE:
				return new PointDefense(player, id, pos, rot, buildStatus, Unit::State::STAND_GROUND);
			case UnitClass::EXTRACTOR:
				return new Extractor(player, id, pos, rot, buildStatus);
			case UnitClass::LAB:
				return new ResearchStruct(player, id, pos, rot, buildStatus);
			case UnitClass::TRADE_CENTER:
			case UnitClass::REFINERY:
				return new Structure(player, id, pos, rot, buildStatus, Unit::State::STAND_GROUND);
			case UnitClass::ENGINEER:
				return new Engineer(player, id, pos, rot, Unit::State::STAND_GROUND);
			case UnitClass::RESOURCE_ROVER:
				return new ResourceRover(player, id, pos, rot, Unit::State::STAND_GROUND);
			default:
				return new Vehicle(player, id, pos, rot, Unit::State::STAND_GROUND);
		}
	}

	Projectile* GameObjectFactory::createProjectile(Unit *unit, int id, Vector3 pos, Quaternion rot){
		sol::state_view SOL_LUA_VIEW = generateView();
		int projectileClass = SOL_LUA_VIEW["projectiles"][id + 1]["projectileClass"];

		Order::Target target = unit->getOrder(0).targets[0];
		Vector3 targetPos = (target.unit ? target.unit->getPos() : target.pos);

		switch((ProjectileClass)projectileClass){
			case ProjectileClass::SHELL:
				return new Shell(unit, id, pos, rot);
			case ProjectileClass::CRUISE_MISSILE:
				return new CruiseMissile(unit, targetPos, pos, rot);
			case ProjectileClass::MISSILE:
				return new Missile(unit, id, targetPos, pos, rot);
			default:
				return new Projectile(unit, id, pos, rot);
		}
	}
	
	ResourceDeposit* GameObjectFactory::createResourceDeposit(Player *player, int id, Vector3 pos, Quaternion rot, int initAmmount){
		return new ResourceDeposit(player, id, pos, rot, initAmmount);
	}
}
