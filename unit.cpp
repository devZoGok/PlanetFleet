#include <box.h>
#include <quad.h>
#include <model.h>
#include <light.h>
#include <texture.h>
#include <material.h>
#include <particleEmitter.h>

#include <stateManager.h>
#include <solUtil.h>

#include <glm.hpp>
#include <ext.hpp>

#include "unit.h"
#include "weapon.h"
#include "util.h"
#include "game.h"
#include "map.h"
#include "vehicle.h"
#include "destructable.h"
#include "gameObjectFactory.h"
#include "activeGameState.h"
#include "gameManager.h"
#include "projectile.h"
#include "defConfigs.h"
#include "pathfinder.h"
#include "ability.h"

using namespace glm;
using namespace vb01;
using namespace gameBase;
using namespace std;

namespace battleship{
	//TODO move restartTime to unitData.lua
    Unit::Unit(Player *player, int id, Vector3 pos, Quaternion rot, State st) : 
		GameObject(GameObject::Type::UNIT, id, player, pos, rot), 
		state(st),
		restartTime(2000)
	{
		selectable = true;

		destructable = new Destructable(this);

		Unit::initProperties();
		initModel();
		initHitbox();
		initSound();
		initWeapons();
        placeAt(pos);
		orientAt(rot);

		Vector2 size = Vector2(lenHpBar, 10);
		hpBackgroundNode = destructable->createBar(Vector2::VEC_ZERO, size, Vector4(0, 0, 0, 1));
		hpForegroundNode = destructable->createBar(Vector2::VEC_ZERO, size, Vector4(0, 1, 0, 1));
    }

    Unit::~Unit() {
		Map::getSingleton()->unblockCells(this);
		destructable->removeBar(hpBackgroundNode);
		destructable->removeBar(hpForegroundNode);
		destroyWeapons();
		destroySound();
		destroyHitbox();
		destroyModel();
    }

	void Unit::initProperties(){
		GameObject::initProperties();

		sol::state_view SOL_LUA_VIEW = generateView();
		string objType = GameObject::getGameObjTableName();
		sol::table unitTable = SOL_LUA_VIEW[objType][id + 1];

		Game *game = Game::getSingleton();
		vector<int> currTechs = player->getTechnologies();

		string name = unitTable["name"];
		alignToSurface = unitTable["alignToSurface"].get_or(false);
        vehicle = unitTable["isVehicle"];

        lineOfSight = unitTable["lineOfSight"]; lineOfSight += game->calcAbilFromTech(Ability::Type::LINE_OF_SIGHT, currTechs, (int)GameObject::type, id);
        unitClass = (UnitClass)unitTable["unitClass"];
		type = (UnitType)unitTable["unitType"];

		guiScreen = "";
		string gsk = "guiScreen";
		sol::optional<string> nameOpt = unitTable[gsk];

		if(nameOpt != sol::nullopt) guiScreen = unitTable[gsk];

		string tblName = "garrisonCapacity";
		sol::optional<sol::table> gc = unitTable[tblName];

		if(gc != sol::nullopt){
			string varName = "numGarrisonSlots";
			SOL_LUA_VIEW.script(varName + " = #" + objType + "[" + to_string(id + 1) + "]." + tblName);
			int numGarrisonSlots = SOL_LUA_VIEW[varName];

			for(int i = 0; i < numGarrisonSlots; i++){
				Vector2 size = 10 * Vector2::VEC_IJ;
				Vector2 pos = Vector2(1.5 * size.x * i, 20);
				Node *bg = destructable->createBar(pos, size, Vector4(0, 0, 0, 1));
				Node *fg = destructable->createBar(pos, size, Vector4(0, 1, 0, 1));
				int category = unitTable[tblName][i + 1];
				garrisonSlots.push_back(GarrisonSlot(bg, fg, pos, category));
			}
		}

		tblName = "buildableUnits";
		sol::optional<sol::table> bu = unitTable[tblName];

		if(bu != sol::nullopt){
			SOL_LUA_VIEW.script("numBuildableUnits = #units[" + to_string(id + 1) + "]." + tblName);
			int numBuildableUnits = SOL_LUA_VIEW["numBuildableUnits"];

			for(int i = 0; i < numBuildableUnits; i++){
				sol::table buTable = unitTable[tblName][i + 1];
				buildableUnits.push_back(BuildableUnit(buTable["id"], game->isUnitUnlocked(currTechs, id) | (bool)buTable["buildable"]));
			}
		}
	}

	void Unit::initWeapons(){
		sol::state_view SOL_STATE_VIEW = generateView();
		string objType = GameObject::getGameObjTableName();
		sol::table unitTable = SOL_STATE_VIEW[objType][id + 1];

		string tblName = "weapons";
		sol::optional<sol::table> weaponsTblOpt = unitTable[tblName];

		if(weaponsTblOpt != sol::nullopt){
			string varName = "numWeapons";
			SOL_STATE_VIEW.script(varName + " = #" + objType + "[" + to_string(id + 1) + "]." + tblName);
			int numWeapons = SOL_STATE_VIEW[varName];

			for(int i = 0; i < numWeapons; i++){
				int wt = 0;
				sol::optional<int> wtOpt = unitTable[tblName][i + 1]["type"];

				if(wtOpt != sol::nullopt) wt = unitTable[tblName][i + 1]["type"];

				weapons.push_back(new Weapon(this, unitTable, i));
			}
		}
	}

	void Unit::destroyWeapons(){
		for(Weapon *weapon : weapons)
			delete weapon;

		weapons.clear();
	}

	void Unit::initLosLight(){
		Light *light = new Light(Light::Type::POINT);
		light->setAttenuation(Light::Attenuation::NONE);
		light->setRadius(lineOfSight);
		light->setColor(Vector3::VEC_IJK * .3);
		light->setAdditiveLighting(false);
		light->setUseAngle(false);

		losLightNode = new Node(Vector3::VEC_J * 5);
		//losLightNode->addLight(light);
		model->attachChild(losLightNode);
	}

	void Unit::destroyLosLight(){
		model->dettachChild(losLightNode);
		delete losLightNode;
		losLightNode = nullptr;
	}
	
	void Unit::destroySound(){
		selectionSfx->stop();
		delete selectionSfx;
		delete selectionSfxBuffer;
	}

	//TODO factor out initSound()
	void Unit::initSound(){
		GameObject::initSound();
		selectionSfxBuffer = new sf::SoundBuffer();
		string sfxPath = generateView()[getGameObjTableName()][id + 1]["selectionSfx"];
		selectionSfx = GameObject::prepareSfx(selectionSfxBuffer, sfxPath);
	}

	void Unit::reinit(){
		if(losLightNode)
			destroyLosLight();

		destroyWeapons();
		destroyHitbox();
		destroyModel();
		destroySound();

		Unit::initProperties();

		initModel();
		initHitbox();
		initSound();
		initWeapons();

		GameObject::reinit();
	}

	bool Unit::canGarrison(Vehicle *vehicle){
		for(int i = 0; i < garrisonSlots.size(); i++)
			if(!garrisonSlots[i].vehicle && garrisonSlots[i].category >= vehicle->getGarrisonCategory())
				return true;

		return false;
	}

	void Unit::launch(Order order){
		getWeaponsByOrder(Order::TYPE::LAUNCH)[0]->fire(order);
		removeOrder(0);
	}

	float Unit::calculateRotation(Vector3 dir, float angle, float maxTurnAngle){
		float rotSpeed = (maxTurnAngle > angle ? angle : maxTurnAngle); 

		if(isTargetToTheRight(dir, leftVec))
			rotSpeed *= -1;

		return rotSpeed;
	}

	void Unit::renderOrderLine(bool mainPlayerSelecting){
        if (!orders.empty() && orders[0].lineId != -1 && mainPlayerSelecting){
			bool display = canDisplayOrderLine();
			LineRenderer *lineRenderer = LineRenderer::getSingleton();
			lineRenderer->toggleVisibility(orders[0].lineId, display);

			if(display){
				lineRenderer->changeLineField(orders[0].lineId, pos, LineRenderer::START);
				Vector3 targPos = (orders[0].targets[0].unit ? orders[0].targets[0].unit->getPos() : orders[0].targets[0].pos);
				lineRenderer->changeLineField(orders[0].lineId, targPos, LineRenderer::END);
			}
        }
	}

	//TODO clean up method code
	void Unit::autoAttackTargets(){
		if(!orders.empty()) return;

		vector<Player*> players = Game::getSingleton()->getPlayers();
		vector<GameObject*> targets;

		for(Player *pl : players)
			if(!(pl == player || pl->getTeam() == player->getTeam())){
				vector<GameObject*> targs = pl->getDestructables();
				targets.insert(targets.end(), targs.begin(), targs.end());
			}

		for(GameObject *targ : targets)
			if(targ->getPos().getDistanceFrom(pos) < lineOfSight){
				receiveOrder(Order(Order::TYPE::ATTACK, vector<Order::Target>{Order::Target(targ)}, Vector3::VEC_ZERO, -1, false), false);
				break;
			}
	}

    void Unit::update() {
		GameObject::update();
		destructable->update();

		if(condition == Condition::EM_JAMMED && getTime() - lastJamTime > restartTime)
			condition = Condition::ABLE;

		if(destructable->getFreezeStatus() >= 100) condition = Condition::FROZEN;

		if(state != State::HOLD_FIRE)
			autoAttackTargets();

		ActiveGameState *activeState = (ActiveGameState*)GameManager::getSingleton()->getStateManager()->getAppStateByType(AppStateType::ACTIVE_STATE);
		Player *mainPlayer = (activeState ? activeState->getPlayer() : nullptr);
		vector<Player*> selectingPlayers = getSelectingPlayers();
		bool mainPlayerSelecting = (find(selectingPlayers.begin(), selectingPlayers.end(), mainPlayer) != selectingPlayers.end());
		bool mainPlayerOwner = (player == mainPlayer);
		bool renderSelectables = (mainPlayerSelecting && mainPlayerOwner);
		renderOrderLine(renderSelectables);

        executeOrders();
		destructable->displayStats(hpForegroundNode, hpBackgroundNode, destructable->getHealth(), destructable->getMaxHealth(), mainPlayerSelecting);

		for(GarrisonSlot &slot : garrisonSlots)
			destructable->displayStats(slot.foreground, slot.background, (int)((bool)slot.vehicle), (int)true, renderSelectables, slot.offset);

		for(Weapon *weapon : weapons)
			weapon->update();
    }

	//TODO remove order argument from action methods
    void Unit::executeOrders() {
		if(orders.empty() || condition != Condition::ABLE) return;

		if(!currOrderStarted) startCurrentOrder();

		switch (orders[0].type) {
		    case Order::TYPE::ATTACK:
		        attack(orders[0]);
		        break;
		    case Order::TYPE::BUILD:
		        build(orders[0]);
		        break;
		    case Order::TYPE::MOVE:
		        move(orders[0]);
		        break;
			case Order::TYPE::GARRISON:
				garrison(orders[0]);
				break;
			case Order::TYPE::EJECT:
				eject(orders[0]);
				break;
		    case Order::TYPE::PATROL:
		        patrol(orders[0]);
		        break;
		    case Order::TYPE::LAUNCH:
		        launch(orders[0]);
		        break;
		        break;
		    case Order::TYPE::LOAD:
		    case Order::TYPE::SUPPLY:
		    case Order::TYPE::UNLOAD:
		        handleResources(orders[0]);
		        break;
		    case Order::TYPE::HACK:
		        hack(orders[0]);
		        break;
		    default:
		        break;
		}
    }

	void Unit::eject(Order order){
		Map *map = Map::getSingleton();
		int currCellId = map->getCellId(pos);
		vector<Map::Cell> &cells = map->getCells(); 
		int adjWaterCellId = -1, adjLandCellId = -1;

		for(Map::Edge edge : cells[currCellId].edges){
			if(adjLandCellId == -1 && cells[edge.destCellId].type == Map::Cell::LAND)
				adjLandCellId = edge.destCellId;
			if(adjWaterCellId == -1 && cells[edge.destCellId].type == Map::Cell::WATER)
				adjWaterCellId = edge.destCellId;
		}

		for(GarrisonSlot &slot : garrisonSlots)
			if(slot.vehicle){
				bool exitToLandCell = (slot.vehicle->getType() == UnitType::LAND && adjLandCellId != -1);
				bool exitToWaterCell = ((slot.vehicle->getType() == UnitType::SEA_LEVEL || slot.vehicle->getType() == UnitType::UNDERWATER) && adjWaterCellId != -1);
				
				if(exitToLandCell || exitToWaterCell)
					slot.vehicle->exitGarrisonable(cells[exitToLandCell ? adjLandCellId : adjWaterCellId].pos);
			}

		removeOrder(0);
	}

	void Unit::attack(Order order){
		vector<GameObject*> targets;

		for(Player *pl : Game::getSingleton()->getPlayers()){
			vector<GameObject*> targs = pl->getDestructables();
			targets.insert(targets.end(), targs.begin(), targs.end());
		}

		vector<Weapon*> attackWeapons = getWeaponsByOrder(Order::TYPE::ATTACK);

		for(Order::Target &target : orders[0].targets)
			if(target.unit){
				if(find(targets.begin(), targets.end(), target.unit) != targets.end()){
					bool canAttack = false;

					for(Weapon *aw : attackWeapons){
						int tc;

						switch(target.unit->getType()){
							case GameObject::Type::UNIT:
								tc = (int)((Unit*)target.unit)->getType();
								break;
							case GameObject::Type::PROJECTILE:
								tc = (int)((Projectile*)target.unit)->getProjectileClass();
								break;
						}

						if(aw->canAttackTarget((int)target.unit->getType(), tc)) canAttack = true;
					}

					if(!canAttack){
						removeOrder(0);
						return;
					}
				}
				else{
					removeOrder(0);
					return;
				}
			}

		Vector3 targPos = (order.targets[0].unit ? order.targets[0].unit->getPos() : order.targets[0].pos);

		for(Weapon *weapon : attackWeapons)
			weapon->trackTarget(targPos);
	}

	bool Unit::validateOrder(Order order){
		switch (order.type) {
		    case Order::TYPE::ATTACK:{
				GameObject *target = order.targets[0].unit;
				bool destructTarg = (!target || (target && target->getDestructable()));
				return destructTarg && !getWeaponsByOrder(Order::TYPE::ATTACK).empty();
			}
		    case Order::TYPE::BUILD:
				return (unitClass == UnitClass::ENGINEER || unitClass == UnitClass::FREEZER);
		    case Order::TYPE::PATROL:
		    case Order::TYPE::MOVE:
				return vehicle;
			case Order::TYPE::GARRISON:
				return validateGarrisonOrder(order);
			case Order::TYPE::EJECT:
				return !garrisonSlots.empty();
		    case Order::TYPE::LAUNCH:
				return validateLaunchOrder();
		    case Order::TYPE::SUPPLY:
		    case Order::TYPE::LOAD:
		    case Order::TYPE::UNLOAD:
				return unitClass == UnitClass::RESOURCE_ROVER;
		    case Order::TYPE::HACK:
				return !getWeaponsByOrder(Order::TYPE::HACK).empty();
			default:
				return false;
		}
	}

	vector<Weapon*> Unit::getWeaponsByOrder(Order::TYPE type){
		vector<Weapon*> weaps;

		for(Weapon *w : weapons)
			if(w->getOrderType() == type)
				weaps.push_back(w);

		return weaps;
	}

	vector<Weapon*> Unit::getWeaponsByType(int type){
		vector<Weapon*> weaps;

		for(Weapon *w : weapons)
			if(w->getType() == (Weapon::Type)type)
				weaps.push_back(w);

		return weaps;
	}

    void Unit::receiveOrder(Order order, bool add) {
		if(!validateOrder(order)) return;

		if(!add){
			while (!orders.empty())
				removeOrder(0);
			
			orderLineDispTime = getTime();
		}

		orders.push_back(order);
    }

    void Unit::halt() {
        while (orders.size() > 0)
            removeOrder(orders.size() - 1);
    }

	void Unit::updateGarrison(Vehicle *garrVeh, bool entering){
		for(GarrisonSlot &slot : garrisonSlots){
			if(entering && !slot.vehicle){
				slot.vehicle = garrVeh;
				break;
			}
			else if(!entering && slot.vehicle == garrVeh){
				slot.vehicle = nullptr;
				break;
			}
		}
	}

	//TODO select only the closest cells based on unit size
	void Unit::placeAt(Vector3 p){
		Map *map = Map::getSingleton();

		if(alignToSurface){
			vector<RayCaster::CollisionResult> res = RayCaster::cast(Vector3(p.x, 100, p.z), -Vector3::VEC_J, map->getNodeParent()->getChildren(), 0, 20);
			
			if(res.empty() || res[0].mesh->getNode() != map->getNodeParent()->getChild(0))
				model->lookAt(Vector3(dirVec.x, 0, dirVec.z).norm(), Vector3::VEC_J);
			else if(res[0].mesh->getNode() == map->getNodeParent()->getChild(0)){
				float angle = upVec.getAngleBetween(res[0].norm);

				//if(angle > 0)
					model->lookAt(leftVec.cross(res[0].norm), res[0].norm);
			}
		}

		ActiveGameState *activeState = (ActiveGameState*)GameManager::getSingleton()->getStateManager()->getAppStateByType(AppStateType::ACTIVE_STATE);

		//check twice in case the unit is warped over a long distance
		if(activeState) map->blockCells(this);
		GameObject::placeAt(p);

		if(activeState){
			map->blockCells(this);

			if(activeState->getPlayer())
				Map::Minimap::getSingleton()->updateImage();
		}
	}

	vector<Player*> Unit::getSelectingPlayers(){
		vector<Player*> players = Game::getSingleton()->getPlayers(), selectingPlayers;

		for(Player *pl : players){
			int numSelectedUnits = pl->getNumSelectedUnits();

			for(int i = 0; i < numSelectedUnits; i++)
				if(pl->getSelectedUnit(i) == this){
					selectingPlayers.push_back(pl);
					break;
				}
		}

		return selectingPlayers;
	}
    
    void Unit::removeOrder(int id) {
		if(orders[id].lineId != -1)
			LineRenderer::getSingleton()->removeLine(orders[id].lineId);

        orders.erase(orders.begin() + id);
		currOrderStarted = false;
    }

    void Unit::select() {
		ActiveGameState *activeState = (ActiveGameState*)GameManager::getSingleton()->getStateManager()->getAppStateByType(AppStateType::ACTIVE_STATE);
		Player *mainPlayer = (activeState ? activeState->getPlayer() : nullptr);

		vector<Player*> selectingPlayers = getSelectingPlayers();
		bool mainPlayerSelecting = (activeState && find(selectingPlayers.begin(), selectingPlayers.end(), mainPlayer) != selectingPlayers.end());

        if(mainPlayer == player && mainPlayerSelecting && selectionSfx)
            selectionSfx->play();

        orderLineDispTime = getTime();
    }
}
