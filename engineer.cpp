#include "engineer.h"
#include "activeGameState.h"
#include "destructable.h"
#include "structure.h"
#include "player.h"
#include "game.h"
#include "map.h"

#include <solUtil.h>
#include <stateManager.h>

#include <vector>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace gameBase;

	Engineer::Engineer(Player *player, int id, Vector3 pos, Quaternion rot, Unit::State state) : Vehicle(player, id, pos, rot, state){
		Game *game = Game::getSingleton();
		vector<int> currTechs = player->getTechnologies();
		hackRange = generateView()["units"][id + 1]["hackRange"]; hackRange += game->calcAbilFromTech(Ability::Type::HACK_RANGE, currTechs, (int)GameObject::type, id);

		Vector2 size = Vector2(lenHpBar, 10);
		hackStatusBackground = destructable->createBar(Vector2::VEC_ZERO, size,  Vector4(0, 0, 0, 1));
		hackStatusForeground = destructable->createBar(Vector2::VEC_ZERO, size,  Vector4(1, 0, 1, 1));
	}

	Engineer::~Engineer(){
		destructable->removeBar(hackStatusBackground);
		destructable->removeBar(hackStatusForeground);
	}

	void Engineer::update(){
		Vehicle::update();

		ActiveGameState *activeState = (ActiveGameState*)GameManager::getSingleton()->getStateManager()->getAppStateByType(AppStateType::ACTIVE_STATE);
		Player *mainPlayer = (activeState ? activeState->getPlayer() : nullptr);

		vector<Player*> selectingPlayers = Unit::getSelectingPlayers();
		bool mainPlayerSelecting = (activeState && find(selectingPlayers.begin(), selectingPlayers.end(), mainPlayer) != selectingPlayers.end());

		if(hackStatus < 100)
			destructable->displayStats(hackStatusForeground, hackStatusBackground, hackStatus, 100, mainPlayer == player && mainPlayerSelecting, Vector2(0, -10));
		else{
			hackStatusBackground->setVisible(false);
			hackStatusForeground->setVisible(false);
		}

		if(orders.empty() || orders[0].type != Order::TYPE::HACK)
			hackStatus = 0;
	}

	//TODO change unit colors after faction change
	void Engineer::hack(Order order){
		Unit *targUnit = (Unit*)order.targets[0].unit;
		bool withinRange = (targUnit->getPos().getDistanceFrom(pos) < hackRange);
		int hackRate = int(generateView()["units"][id + 1]["hackTime"]) / 100;
		bool canHack = (getTime() - lastHackTime > hackRate);

		if(withinRange && canHack){
			hackStatus++;
			pursuingTarget = false;
		}
		else if(!withinRange){
			hackStatus = 0;

			if(!pursuingTarget){
				preparePathpoints(order, targUnit->getPos());
				pursuingTarget = true;
			}
			else
				navigateToTarget(.9 * hackRange);
		}

		if(hackStatus >= 100){
			order.targets[0].unit->changePlayer(player);
			removeOrder(0);
		}
	}

	/// @brief Build/Repair structures and vehicles based on the passed order
	/// @param order Must be a structure or vehicle
	void Engineer::build(Order order)
	{
		// By saving the target as a unit it includes vehicles not just structures as vechicle::build only includes
		Unit *targ = (Unit*)order.targets[0].unit;

		// If target is a vehicle or completed structure, then repair the vehicle's/structure's health
		if(targ->isVehicle() || ((Structure*)targ)->isComplete())
		{
			// Check if the engineer is close enough to the vehicle/structure
			// This is the offset between the engineer and vehicle/structure
			float offset = .5 * Map::getSingleton()->getCellSize().x;

			if(pathPoints.empty() && closeEnough(targ, pos, offset))
			{
				sol::table targTable = generateView()["units"][targ->getId()];
				// Is there a different costRate and buildTime (repairTime) for repairing a structure/vehicle instead of building one?
				int costRate = (int)targTable["cost"] / 100, buildRate = (int)targTable["buildTime"] / 100;
				// For if there will be a specific amount of health added
				// int healthAddAmount = (int)targTable["healthRegen"];
			
				// If the player is able is able to rebuild economically and according to build rate and the vehicle/structure is not at full health
				// Increment the health, subtract the used resources and update the latest repair time
				if(!targ->getDestructable()->atFullHealth() && player->getResource(ResourceType::REFINEDS) >= costRate && getTime() - lastRepairTime > buildRate){
					targ->getDestructable()->incrementHealth();
					player->updateResource(ResourceType::REFINEDS, -costRate, true);
					lastRepairTime = getTime();
				}
				// If the vehicle/structure is at full health delete the order
				else if(targ->getDestructable()->atFullHealth() )
				{
					removeOrder(0);
				}
			}
			// If the vehicle/structure is not close enough then navigate the engineer toward the vehicle/structure
			else navigate(offset);
		}
		// If the target is a structure and its not completely built, continue building it
		else if(!((Structure*)targ)->isComplete())
		{
			Vehicle::build(order);
		}
		else
		{
			return;
		}
	}
}
