#include "researchStruct.h"
#include "activeGameState.h"
#include "player.h"
#include "game.h"

#include <stateManager.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace gameBase;

	ResearchStruct::ResearchStruct(Player *player, int id, Vector3 pos, Quaternion rot, int buildStatus, Unit::State state) : Structure(player, id, pos, rot, buildStatus, state){
		sol::table unitTable = generateView()["units"][id + 1];
		generationRate = unitTable["generationRate"];
		generationSpeed = unitTable["generationSpeed"];
		researchCost = unitTable["researchCost"];

		Vector2 size = Vector2(lenHpBar, 10);
		researchStatusBackground = Unit::createBar(Vector2::VEC_ZERO, size,  Vector4(0, 0, 0, 1));
		researchStatusForeground = Unit::createBar(Vector2::VEC_ZERO, size,  Vector4(1, 0, 1, 1));
	}

	ResearchStruct::~ResearchStruct(){
		removeBar(researchStatusBackground);
		removeBar(researchStatusForeground);
	}

	void ResearchStruct::update(){
		Structure::update();

		if(!isComplete()) return;

		ActiveGameState *activeState = (ActiveGameState*)GameManager::getSingleton()->getStateManager()->getAppStateByType(AppStateType::ACTIVE_STATE);
		Player *mainPlayer = (activeState ? activeState->getPlayer() : nullptr);

		vector<Player*> selectingPlayers = Unit::getSelectingPlayers();
		bool mainPlayerSelecting = (activeState && find(selectingPlayers.begin(), selectingPlayers.end(), mainPlayer) != selectingPlayers.end());

		if(researchQueue.empty()){
			if(health > .3 * maxHealth && player->getResource(ResourceType::REFINEDS) >= researchCost && canUpdateResearch()){
				player->updateResource(ResourceType::RESEARCH, generationSpeed, true);
				player->updateResource(ResourceType::REFINEDS, -researchCost, true);
				lastUpdateTime = getTime();
			}

			researchStatusBackground->setVisible(false);
			researchStatusForeground->setVisible(false);
		}
		else if(!researchQueue.empty()){
			Unit::displayUnitStats(researchStatusForeground, researchStatusBackground, researchStatus, 100, mainPlayer == player && mainPlayerSelecting, Vector2(0, -10));

			int techCost = Game::getSingleton()->getTechnology(researchQueue[0]).cost;
			int playerResearch = player->getResource(ResourceType::RESEARCH);

			if(techCost > playerResearch && canUpdateResearch()){
				researchStatus += int(100 * ((float)playerResearch / techCost));
				lastUpdateTime = getTime();
			}
			else if(researchStatus >= 100 || techCost <= playerResearch){
				player->addTechnology(researchQueue[0]);
				researchQueue.erase(researchQueue.begin());
				researchStatus = 0;
			}
		}
	}
}
