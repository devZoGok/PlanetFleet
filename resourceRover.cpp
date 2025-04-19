#include "resourceRover.h"
#include "resourceDeposit.h"
#include "map.h"
#include "game.h"
#include "player.h"
#include "extractor.h"
#include "structure.h"
#include "activeGameState.h"

#include <stateManager.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace gameBase;

	ResourceRover::ResourceRover(Player *player, int id, Vector3 pos, Quaternion rot, Unit::State state) : Vehicle(player, id, pos, rot, state) {
		Vector2 size = Vector2(lenHpBar, 10);
		loadBackground = Unit::createBar(Vector2::VEC_ZERO, size,  Vector4(0, 0, 0, 1));
		loadForeground = Unit::createBar(Vector2::VEC_ZERO, size,  Vector4(1, 1, 0, 1));

		initProperties();
	}

	ResourceRover::~ResourceRover(){
		removeBar(loadForeground);
		removeBar(loadBackground);
	}

	void ResourceRover::initProperties(){
		Game *game = Game::getSingleton();
		vector<int> currTechs = player->getTechnologies();

		sol::table unitTable = generateView()["units"][id + 1];
		capacity = unitTable["capacity"]; capacity += game->calcAbilFromTech(Ability::Type::CAPACITY, currTechs, (int)GameObject::type, id);
		loadSpeed = unitTable["loadSpeed"]; loadSpeed += game->calcAbilFromTech(Ability::Type::LOAD_SPEED, currTechs, (int)GameObject::type, id);
		loadRate = unitTable["loadRate"]; loadRate += game->calcAbilFromTech(Ability::Type::LOAD_RATE, currTechs, (int)GameObject::type, id);
	}

	//TODO equate rover load and extractor draw rates
	void ResourceRover::collectRefineds(Order order, float minDist){
		vector<Unit*> units = player->getUnits();
		vector<Structure*> extractors, refineries;

		for(Unit *unit : units){
			if(unit->getUnitClass() == UnitClass::EXTRACTOR && ((Extractor*)unit)->getDeposit()->getAmmount() > 0)
				extractors.push_back((Structure*)unit);
			else if(unit->getUnitClass() == UnitClass::REFINERY)
				refineries.push_back((Structure*)unit);
		}

		nearestExtractor = (Extractor*)getClosestUnit(extractors);
		nearestRefinery = getClosestUnit(refineries);

		if(nearestExtractor && nearestExtractor->getPos().getDistanceFrom(pos) <= minDist){
			ResourceDeposit *deposit = nearestExtractor->getDeposit();

			if(canLoad() && deposit->getAmmount() > 0){
				if(nearestExtractor->canDraw()){
					nearestExtractor->draw();
					load[(int)ResourceType::REFINEDS] += loadSpeed;
					lastLoadTime = getTime();
				}
			}
			else if(calcTotalLoad() == capacity && nearestRefinery)
				preparePathpoints(order, nearestRefinery->getPos(), true);
		}
		else if(!nearestExtractor){
			if(nearestRefinery && load[(int)ResourceType::REFINEDS] > 0)
				preparePathpoints(order, nearestRefinery->getPos(), true);
			else
				removeOrder(0);
		}

		if(nearestRefinery && nearestRefinery->getPos().getDistanceFrom(pos) <= minDist){
			if(canUnload((int)ResourceType::REFINEDS)){
				load[(int)ResourceType::REFINEDS] -= loadSpeed;
				player->updateResource(ResourceType::REFINEDS, loadSpeed, true);
				lastLoadTime = getTime();
			}
			else if(load[(int)ResourceType::REFINEDS] == 0 && nearestExtractor)
				preparePathpoints(order, nearestExtractor->getPos());
		}
	}

	void ResourceRover::transferResource(Structure *targStruct, float minDist){
		bool closeEnough = (pos.getDistanceFrom(targStruct->getPos()) <= minDist);
		ResourceType resType;

		switch(targStruct->getUnitClass()){
			case UnitClass::TRADE_CENTER:
				resType = ResourceType::WEALTH;
				break;
			case UnitClass::REFINERY:
				resType = ResourceType::REFINEDS;
				break;
			case UnitClass::LAB:
				resType = ResourceType::RESEARCH;
				break;
		}

		if(closeEnough){
			if(player == targStruct->getPlayer() && canLoad() && player->getResource(resType) > 0){
				player->updateResource(resType, -loadSpeed, true);
				load[(int)resType] += loadSpeed;
				lastLoadTime = getTime();
			}
			else if(player != targStruct->getPlayer() && canUnload((int)resType)){
				targStruct->getPlayer()->updateResource(resType, loadSpeed, true);
				load[(int)resType] -= loadSpeed;
				lastLoadTime = getTime();
			}
		}
	}

	void ResourceRover::supply(Order order){
		float minDist = .5 * Map::getSingleton()->getCellSize().x;
		
		if(!pathPoints.empty())
			navigate(minDist);
		else{
			Unit *targUnit = order.targets[0].unit;

			if(targUnit->getUnitClass() == UnitClass::EXTRACTOR)
				collectRefineds(order, minDist);
			else transferResource((Structure*)targUnit, minDist);
		}
	}

	void ResourceRover::update(){
		Vehicle::update();

		vector<Unit*> units = player->getUnits();
		nearestExtractor = (find(units.begin(), units.end(), nearestExtractor) != units.end() ? nearestExtractor : nullptr);
		nearestRefinery = (find(units.begin(), units.end(), nearestRefinery) != units.end() ? nearestRefinery : nullptr);

		ActiveGameState *activeState = (ActiveGameState*)GameManager::getSingleton()->getStateManager()->getAppStateByType(AppStateType::ACTIVE_STATE);
		Player *mainPlayer = (activeState ? activeState->getPlayer() : nullptr);

		vector<Player*> selectingPlayers = getSelectingPlayers();
		bool mainPlayerSelecting = (activeState && find(selectingPlayers.begin(), selectingPlayers.end(), mainPlayer) != selectingPlayers.end());

		Unit::displayUnitStats(loadForeground, loadBackground, calcTotalLoad(), capacity, mainPlayer == player && mainPlayerSelecting, Vector2(0, -10));
	}

	Unit* ResourceRover::getClosestUnit(vector<Structure*> structs){
		if(structs.empty()) return nullptr;

		int minDistId = -1;

		for(int i = 0; i < structs.size(); i++)
			if(structs[i]->isComplete()){
				minDistId = i;
				break;
			}

		if(minDistId == -1) return nullptr;

		for(int i = 0; i < structs.size(); i++)
			if(structs[i]->isComplete() && structs[minDistId]->getPos().getDistanceFrom(pos) > structs[i]->getPos().getDistanceFrom(pos))
				minDistId = i;

		return structs[minDistId];
	}
}
