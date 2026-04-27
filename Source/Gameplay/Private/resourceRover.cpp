#include "resourceRover.h"
#include "resourceDeposit.h"
#include "destructable.h"
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
		loadBackground = destructable->createBar(Vector2::VEC_ZERO, size,  Vector4(0, 0, 0, 1));
		loadForeground = destructable->createBar(Vector2::VEC_ZERO, size,  Vector4(1, 1, 0, 1));

		initProperties();
	}

	ResourceRover::~ResourceRover(){
		destructable->removeBar(loadForeground);
		destructable->removeBar(loadBackground);
	}

	void ResourceRover::initProperties(){
		Game *game = Game::getSingleton();
		vector<int> currTechs = player->getTechnologies();

		sol::table unitTable = generateView()["units"][id + 1];
		capacity = unitTable["capacity"]; capacity += game->calcAbilFromTech(Ability::Type::CAPACITY, currTechs, (int)GameObject::type, id);
		loadSpeed = unitTable["loadSpeed"]; loadSpeed += game->calcAbilFromTech(Ability::Type::LOAD_SPEED, currTechs, (int)GameObject::type, id);
		loadRate = unitTable["loadRate"]; loadRate += game->calcAbilFromTech(Ability::Type::LOAD_RATE, currTechs, (int)GameObject::type, id);
	}

	//TODO implement a check of whether a vehicle is next to a building's outline
	void ResourceRover::loadResources(Structure *targStruct, bool loadResource){
		float w = targStruct->getWidth(), l = targStruct->getLength();
		bool closeEnough = (pos.getDistanceFrom(targStruct->getPos()) <= sqrt(l * l + w * w));
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

	//TODO equate rover load and extractor draw rates
	void ResourceRover::collectRefineds(Order order){
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

		auto closeEnough = [](GameObject *obj, Vector3 pos){
			Vector3 neVec = pos - obj->getPos();
			float angle = obj->getDirVec().getAngleBetween(neVec.norm());

			if(angle > PI / 2) angle = PI - angle;

			return cos(angle) * neVec.getLength() < .5 * obj->getLength();
		};

		if(nearestExtractor && closeEnough(nearestExtractor, pos)){
			ResourceDeposit *deposit = nearestExtractor->getDeposit();

			if(canLoad() && deposit->getAmmount() > 0){
				if(nearestExtractor->canDraw()){
					nearestExtractor->draw();
					cargo[(int)ResourceType::REFINEDS] += loadSpeed;
					lastLoadTime = getTime();
				}
			}
			else if(calcTotalLoad() == capacity && nearestRefinery){
				order.targets[0].unit = nearestRefinery;
				preparePathpoints(order, nearestRefinery->getPos());
			}
		}
		else if(!nearestExtractor){
			if(nearestRefinery && cargo[(int)ResourceType::REFINEDS] > 0){
				order.targets[0].unit = nearestRefinery;
				preparePathpoints(order, nearestRefinery->getPos());
			}
			else removeOrder(0);
		}

		if(nearestRefinery && closeEnough(nearestRefinery, pos)){
			if(canUnload((int)ResourceType::REFINEDS)){
				cargo[(int)ResourceType::REFINEDS] -= loadSpeed;
				player->updateResource(ResourceType::REFINEDS, loadSpeed, true);
				lastLoadTime = getTime();
			}
			else if(cargo[(int)ResourceType::REFINEDS] == 0 && nearestExtractor){
				order.targets[0].unit = nearestExtractor;
				preparePathpoints(order, nearestExtractor->getPos());
			}
		}
	}

	void ResourceRover::handleResources(Order order){
		if(!pathPoints.empty()) navigate(.01);
		else if(order.type == Order::TYPE::SUPPLY)
			collectRefineds(order);
		else
			loadResources((Structure*)order.targets[0].unit, order.type == Order::TYPE::LOAD);
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

		destructable->displayStats(loadForeground, loadBackground, calcTotalLoad(), capacity, mainPlayer == player && mainPlayerSelecting, Vector2(0, -10));
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
