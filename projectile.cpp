#include <node.h>
#include <model.h>
#include <material.h>
#include <quaternion.h>
#include <particleEmitter.h>

#include <stateManager.h>

#include "game.h"
#include "unit.h"
#include "util.h"
#include "player.h"
#include "environment.h"
#include "projectile.h"
#include "defConfigs.h"
#include "destructable.h"
#include "resourceDeposit.h"
#include "inGameAppState.h"

using namespace std;
using namespace vb01;

namespace battleship{
	using namespace vb01;
	using namespace configData;
	using namespace gameBase;

    Projectile::Projectile(Unit *un, int id, Vector3 pos, Quaternion rot) : GameObject(GameObject::Type::PROJECTILE, id, un->getPlayer(), pos, rot), unit(un), initPos(pos){
		initProperties();
		initModel();
		initSound();
		placeAt(pos);
		orientAt(rot);
    }

    Projectile::~Projectile(){
		destroySound();
		destroyModel();
	}

	void Projectile::reinit(){
		destroySound();
		destroyModel();

		initProperties();

		initModel();

		GameObject::reinit();
	}

	void Projectile::initProperties(){
		GameObject::initProperties();
		Game *game = Game::getSingleton();
		vector<int> currTechs = player->getTechnologies();

		sol::table projTable = generateView()[GameObject::getGameObjTableName()][id + 1];
		projClass = (ProjectileClass)projTable["projectileClass"];

        rayLength = projTable["rayLength"];
        directHitDamage = projTable["directHitDamage"]; directHitDamage += game->calcAbilFromTech(Ability::Type::DIRECT_HIT_DAMAGE, currTechs, (int)GameObject::type, id);

		string explKey = "explosion";
        explosionDamage = projTable[explKey]["damage"]; explosionDamage += game->calcAbilFromTech(Ability::Type::EXPLOSION_DAMAGE, currTechs, (int)GameObject::type, id);
        explosionRadius = projTable[explKey]["radius"]; explosionRadius += game->calcAbilFromTech(Ability::Type::EXPLOSION_RADIUS, currTechs, (int)GameObject::type, id);
        speed = projTable["speed"];
		rotAngle = projTable["rotAngle"].get_or(0.0);
	}

    void Projectile::update() {
		GameObject::update();
		placeAt(pos + speed * dirVec);
    }

	void Projectile::detonate(Unit *target){
		if(target) target->getDestructable()->takeDamage(directHitDamage);

		sol::table tbl = generateView()[getGameObjTableName()][id + 1]["explosion"];
		FxManager::Fx *fx = FxManager::getSingleton()->initFx(tbl["fx"], model, false, pos);
		Environment::explode(fx, (Environment::Detonation)tbl["detonation"], pos, tbl["damage"], tbl["radius"]);

		remove = true;
	}

	void Projectile::checkUnitCollision(){
		vector<Player*> players = Game::getSingleton()->getPlayers(true);
		vector<Unit*> targetUnits;
		vector<Node*> targetNodes;

		for(Player *pl : players){
			vector<Unit*> units = pl->getUnits();

			for(Unit *u : units)
				if(unit && unit != u){
					targetUnits.push_back(u);
					targetNodes.push_back(u->getHitbox());
				}
		}

		vector<RayCaster::CollisionResult> results = RayCaster::cast(pos, dirVec, targetNodes, rayLength);

		if(!results.empty())
			for(int i = 0; i < targetNodes.size(); i++)
				if(targetNodes[i]->getMesh(0) == results[0].mesh)
					detonate(targetUnits[i]);
	}

	//TODO fix map boundary checks
	void Projectile::checkSurfaceCollision(bool useBottomCell){
		Map *map = Map::getSingleton();
		vector<Map::Cell> &cells = map->getCells();
		int cellId = map->getCellId(pos, false);
		Vector3 mapSize = map->getMapSize();

		if(cellId < 0 || fabs(pos.x) > .5 * mapSize.x || fabs(pos.z) > .5 * mapSize.z){
			remove = true;
			return;
		}

		if(useBottomCell && !cells[cellId].underWaterCellIds.empty()){
			int numUnderwaterCells = cells[cellId].underWaterCellIds.size();
			cellId = cells[cellId].underWaterCellIds[numUnderwaterCells - 1];
		}

		if((pos + dirVec * rayLength).y <= cells[cellId].pos.y + .5 * map->getCellSize().y)
			detonate();
	}

    void Projectile::debug(){
    }
}
