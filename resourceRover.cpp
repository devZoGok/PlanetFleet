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

	void ResourceRover::handleTransfer(Structure *targStruct, float minDist, bool loadResource){
		bool closeEnough = (pos.getDistanceFrom(targStruct->getPos()) <= minDist);
		if(!closeEnough) return;

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

		if(player == targStruct->getPlayer()){
			if(!loadResource && canUnload((int)resType)){
				player->updateResource(resType, loadSpeed, true);
				cargo[(int)resType] -= loadSpeed;
				lastLoadTime = getTime();
			}
			else if(loadResource && canLoad() && player->getResource(resType) > 0){
				player->updateResource(resType, -loadSpeed, true);
				cargo[(int)resType] += loadSpeed;
				lastLoadTime = getTime();
			}
		}
		else{
			vector<TradeOffer*> offers = player->getTradeOffers(targStruct->getPlayer());
			if(offers.empty()) return;

			TradeOffer *offer = offers[0];

			if(!loadResource && offer->tradeResources[(int)resType][1] > offer->deliveredResources[(int)resType][1] && canUnload((int)resType)){
				targStruct->getPlayer()->updateResource(resType, loadSpeed, true);
				offer->deliveredResources[(int)resType][1]++;
				cargo[(int)resType] -= loadSpeed;
				lastLoadTime = getTime();
			}
			else if(
					loadResource &&
					offer->tradeResources[(int)resType][0] > offer->deliveredResources[(int)resType][0] &&
					targStruct->getPlayer()->getResource(resType) > 0 &&
					canLoad()
				)
			{
				targStruct->getPlayer()->updateResource(resType, -loadSpeed, true);
				offer->deliveredResources[(int)resType][0]++;
				cargo[(int)resType] += loadSpeed;
				lastLoadTime = getTime();
			}
		}
	}

	void ResourceRover::transfer(Order order, bool loadResource){
		float minDist = .5 * Map::getSingleton()->getCellSize().x;
		
		if(!pathPoints.empty())
			navigate(minDist);
		else
			handleTransfer((Structure*)order.targets[0].unit, minDist, loadResource);
	}

	void ResourceRover::load(Order order){transfer(order, true);}

	void ResourceRover::unload(Order order){transfer(order, false);}

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
					cargo[(int)ResourceType::REFINEDS] += loadSpeed;
					lastLoadTime = getTime();
				}
			}
			else if(calcTotalLoad() == capacity && nearestRefinery)
				preparePathpoints(order, nearestRefinery->getPos(), true);
		}
		else if(!nearestExtractor){
			if(nearestRefinery && cargo[(int)ResourceType::REFINEDS] > 0)
				preparePathpoints(order, nearestRefinery->getPos(), true);
			else
				removeOrder(0);
		}

		if(nearestRefinery && nearestRefinery->getPos().getDistanceFrom(pos) <= minDist){
			if(canUnload((int)ResourceType::REFINEDS)){
				cargo[(int)ResourceType::REFINEDS] -= loadSpeed;
				player->updateResource(ResourceType::REFINEDS, loadSpeed, true);
				lastLoadTime = getTime();
			}
			else if(cargo[(int)ResourceType::REFINEDS] == 0 && nearestExtractor)
				preparePathpoints(order, nearestExtractor->getPos());
		}
	}

	void ResourceRover::supply(Order order){
		float minDist = .5 * Map::getSingleton()->getCellSize().x;
		
		if(!pathPoints.empty())
			navigate(minDist);
		else
			collectRefineds(order, minDist);
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
