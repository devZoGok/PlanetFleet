#include <algorithm>

#include <stateManager.h>
#include <inputManager.h>
#include <soundManager.h>

#include <assetManager.h>
#include <root.h>

#include "gameManager.h"
#include "gameObjectFrameController.h"
#include "gameObjectFactory.h"
#include "map.h"
#include "game.h"
#include "guiAppState.h"
#include "defConfigs.h"
#include "player.h"
#include "extractor.h"
#include "factory.h"
#include "vehicle.h"
#include "engineer.h"
#include "destructable.h"
#include "pointDefense.h"
#include "pathfinder.h"
#include "resourceDeposit.h"

using namespace std;
using namespace vb01;
using namespace gameBase;

namespace battleship{
	static GameManager *gameManager = nullptr;

	GameManager* GameManager::getSingleton(){
		if(!gameManager)
			gameManager = new GameManager();

		return gameManager;
	}

	void GameManager::registerMembers(){
		sol::state_view SOL_LUA_STATE = generateView();

		SOL_LUA_STATE.new_usertype<Order>(
			"Order", sol::constructors<Order(), Order(Order::TYPE, vector<Order::Target>, Vector3, int)>(),
			"type", &Order::type,
			"direction", &Order::direction,
			"targets", &Order::targets
		);

		SOL_LUA_STATE.new_usertype<Order::Target>(
			"Target", sol::constructors<Order::Target(), Order::Target(GameObject*, Vector3)>(),
			"unit", &Order::Target::unit,
			"pos", &Order::Target::pos
		);

		SOL_LUA_STATE.new_usertype<GameObjectFactory>(
			"GameObjectFactory",
			"createUnit", &GameObjectFactory::createUnit,
			"createResourceDeposit", &GameObjectFactory::createResourceDeposit
		);

		SOL_LUA_STATE.new_usertype<Unit::GarrisonSlot>(
			"GarrisonSlot",
			"vehicle", &Unit::GarrisonSlot::vehicle,
			"category", &Unit::GarrisonSlot::category
		);

		SOL_LUA_STATE.new_usertype<Unit>(
			"Unit", sol::constructors<Unit(Player*, int, Vector3, Quaternion)>(),
			"setState", &Unit::setState,
			"getDestructable", &Unit::getDestructable,
			"getGarrisonSlots", &Unit::getGarrisonSlots,
			"getNumFreeGarrisonSlots", &Unit::getNumFreeGarrisonSlots,
			"getNumGarrisonSlots", &Unit::getNumGarrisonSlots,
			"isGarrisonEmpty", &Unit::isGarrisonEmpty,
			"getOrder", &Unit::getOrder,
			"getNumOrders", &Unit::getNumOrders,
			"getPos", &GameObject::getPos,
			"getUnitClass", &Unit::getUnitClass,
			"getLineOfSight", &Unit::getLineOfSight,
			"toEngineer", &Unit::toEngineer,
			"toPointDefense", &Unit::toPointDefense,
			"toStructure", &Unit::toStructure,
			"toFactory", &Unit::toFactory,
			"toGameObject", &Unit::toGameObject,
			"toVehicle", &Unit::toVehicle,
			//"toVehicle", [](Unit *u){return (Vehicle*)u;},
			"getBuildableUnit", &Unit::getBuildableUnit,
			"getBuildableUnits", &Unit::getBuildableUnits,
			"getNumBuildableUnits", &Unit::getNumBuildableUnits
		);

		SOL_LUA_STATE.new_usertype<Vehicle>(
			"Engineer", sol::constructors<Vehicle(Player*, int, Vector3, Quaternion, Unit::State)>(),
			"getGarrisonable", &Vehicle::getGarrisonable
		);

		SOL_LUA_STATE.new_usertype<Engineer>(
			"Engineer", sol::constructors<Engineer(Player*, int, Vector3, Quaternion, Unit::State)>(),
			"getBuildableUnit", &Unit::getBuildableUnit,
			"getBuildableUnits", &Unit::getBuildableUnits,
			"getNumBuildableUnits", &Unit::getNumBuildableUnits
		);

		SOL_LUA_STATE.new_usertype<Structure>(
			"Structure", sol::constructors<Structure(Player*, int, Vector3, Quaternion, int)>(),
			"getPos", &GameObject::getPos,
			"isComplete", &Structure::isComplete,
			"getBuildStatus", &Structure::getBuildStatus
		);

		SOL_LUA_STATE.new_usertype<BuildableUnit>(
			"BuildableUnit", sol::constructors<BuildableUnit(int, bool)>(),
			"id", &BuildableUnit::id,
			"buildable", &BuildableUnit::buildable
		);

		SOL_LUA_STATE.new_usertype<Factory>(
			"Factory", sol::constructors<Factory(Player*, int, Vector3, Quaternion, int)>(),
			"getBuildableUnit", &Unit::getBuildableUnit,
			"getBuildableUnits", &Unit::getBuildableUnits,
			"getNumBuildableUnits", &Unit::getNumBuildableUnits,
			"getPos", &GameObject::getPos,
			"appendToQueue", &Factory::appendToQueue,
			"getBuildStatus", &Structure::getBuildStatus,
			"getNumQueueUnitsById", &Factory::getNumQueueUnitsById,
			"getQueue", &Factory::getQueue
		);

		SOL_LUA_STATE.new_usertype<Player>(
			"Player", sol::constructors<Player(int, int, int, Vector3, bool, int, string)>(),
			"getSelectedUnits", &Player::getSelectedUnits,
			"haltUnits", &Player::haltUnits,
			"addUnit", &Player::addUnit,
			"getUnit", &Player::getUnit,
			"getNumUnits", &Player::getNumUnits,
			"getResourceDeposits", &Player::getResourceDeposits,
			"issueOrder", &Player::issueOrder,
			"selectUnits", &Player::selectUnits,
			"deselectUnits", &Player::deselectUnits,
			"getSpawnPointId", &Player::getSpawnPointId,
			"getFaction", &Player::getFaction,
			"getTeam", &Player::getTeam,
			"getUnits", &Player::getUnits,
			"getUnitsById", &Player::getUnitsById,
			"getUnitsByClass", &Player::getUnitsByClass
		);

		SOL_LUA_STATE.new_usertype<Extractor>(
			"Extractor", sol::constructors<Extractor(Player*, int, Vector3, Quaternion, int, ResourceDeposit*, Unit::State)>()
		);

		SOL_LUA_STATE.new_usertype<ResourceDeposit>(
			"ResourceDeposit", sol::constructors<ResourceDeposit(Player*, int, Vector3, Quaternion, int)>(),
			"getExtractor", &ResourceDeposit::getExtractor,
			"getPos", &GameObject::getPos
		);

		SOL_LUA_STATE.new_usertype<Game>(
			"Game",
			"getSingleton", &Game::getSingleton,
			"getPlayers", &Game::getPlayers
		);

		SOL_LUA_STATE.new_usertype<Vector3>(
			"Vector3", sol::constructors<Vector3(), Vector3(float, float, float)>(),
			"x", &Vector3::x,
			"y", &Vector3::y,
			"z", &Vector3::z,
			"norm", &Vector3::norm,
			"getAngleBetween", &Vector3::getAngleBetween,
			"getDistanceFrom", &Vector3::getDistanceFrom,
			"neg", [](Vector3 v){return -v;},
			"add", [](Vector3 v1, Vector3 v2){return v1 + v2;},
			"subtr", [](Vector3 v1, Vector3 v2){return v1 - v2;},
			"mult", [](Vector3 v1, float s){return v1 * s;},
			"div", [](Vector3 v1, float s){return v1 / s;}
		);

		SOL_LUA_STATE.new_usertype<Quaternion>(
			"Quaternion", sol::constructors<Quaternion(), Quaternion(float, float, float, float), Quaternion(float, Vector3)>(),
			"w", &Quaternion::w,
			"x", &Quaternion::x,
			"y", &Quaternion::y,
			"z", &Quaternion::z,
			"getAngle", &Quaternion::getAngle,
			"getAxis", &Quaternion::getAxis,
			"multVec", [](Quaternion q, Vector3 v){return q * v;}
		);

		SOL_LUA_STATE.new_usertype<Map::Cell>(
			"Cell",
			"pos", &Map::Cell::pos,
			"type", &Map::Cell::type
		);

		SOL_LUA_STATE.new_usertype<Map>(
			"Map",
			"getSingleton", &Map::getSingleton,
			"getCell", &Map::getCell,
			"getCells", &Map::getCells,
			"getCellId", &Map::getCellId,
			"getMapSize", &Map::getMapSize,
			"getSpawnPoint", &Map::getSpawnPoint,
			"getNumSpawnPoints", &Map::getNumSpawnPoints
		);

		SOL_LUA_STATE.new_usertype<Pathfinder>(
			"Pathfinder",
			"getSingleton", &Pathfinder::getSingleton,
			"calcHeuristics", &Pathfinder::calcHeuristics,
			"findPath", &Pathfinder::findPath
		);

		SOL_LUA_STATE.new_usertype<GameObjectFrame>(
			"GameObjectFrame", sol::constructors<GameObjectFrame(int, GameObject::Type, Player*, Unit*, Vector3, Quaternion)>(),
			"getOriginalUnit", &GameObjectFrame::getOriginalUnit,
			"destroy", &GameObjectFrame::destroy,
			"placeAt", &GameObjectFrame::placeAt,
			"getDirVec", &GameObjectFrame::getDirVec,
			"getLeftVec", &GameObjectFrame::getLeftVec,
			"getLength", &GameObjectFrame::getLength,
			"getWidth", &GameObjectFrame::getWidth,
			"status", &GameObjectFrame::status
		);

		SOL_LUA_STATE.new_usertype<GameObjectFrameController>(
			"GameObjectFrameController",
			"getSingleton", &GameObjectFrameController::getSingleton,
			"checkPlacement", &GameObjectFrameController::checkPlacement
		);
	}

	void GameManager::initLua(string gameDir){
		sol::state_view SOL_LUA_STATE = generateView();

		path = gameDir + "Assets/";
		SOL_LUA_STATE.script("PATH = \"" + path + "\";");

		for(string f : configData::scripts)
			SOL_LUA_STATE.script_file(path + f);

		sol::table resTable = SOL_LUA_STATE["graphics"]["resolution"]; 
		width = resTable["x"];
		height = resTable["y"];
	}

	void GameManager::start(string gameDir) {
		running = true;
		registerMembers();
		initLua(gameDir);

		Root *root = Root::getSingleton();
		root->start(width, height, path + "../external/vb01/", "Battleship");

		stateManager = new StateManager();
    	inputManager = new InputManager(stateManager, root->getWindow());
	}

    void GameManager::update() {
		Root::getSingleton()->update();
		SoundManager::getSingleton()->update();
        inputManager->update();
		stateManager->update();
    }
}
