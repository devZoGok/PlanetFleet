#include "extractor.h"
#include "player.h"
#include "game.h"
#include "destructable.h"
#include "activeGameState.h"
#include "resourceDeposit.h"

#include <stateManager.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace gameBase;

	Extractor::Extractor(Player *player, int id, Vector3 pos, Quaternion rot, int buildStatus, ResourceDeposit *rd, Unit::State state) : Structure(player, id, pos, rot, buildStatus, state){
		Vector2 size = Vector2(lenHpBar, 10);
		ammountBackground = destructable->createBar(Vector2::VEC_ZERO, size,  Vector4(0, 0, 0, 1));
		ammountForeground = destructable->createBar(Vector2::VEC_ZERO, size,  Vector4(0, 0, 1, 1));

		if(rd) return;

		initProperties();

		for(ResourceDeposit *dep : Game::getSingleton()->getCivilianPlayer()->getResourceDeposits())
			if(dep->getPos().getDistanceFrom(pos) < .001){
				deposit = dep;
				deposit->setExtractor(this);
				break;
			}
	}

	Extractor::~Extractor(){
		if(deposit) deposit->setExtractor(nullptr);

		destructable->removeBar(ammountForeground);
		destructable->removeBar(ammountBackground);
	}

	void Extractor::initProperties(){
		Game *game = Game::getSingleton();
		vector<int> currTechs = player->getTechnologies();

		sol::table unitTable = generateView()["units"][id + 1];
		drawRate = unitTable["drawRate"]; drawRate += game->calcAbilFromTech(Ability::Type::DRAW_RATE, currTechs, (int)GameObject::type, id);
		drawSpeed = unitTable["drawSpeed"]; drawSpeed += game->calcAbilFromTech(Ability::Type::DRAW_SPEED, currTechs, (int)GameObject::type, id);
	}

	void Extractor::update(){
		Structure::update();

		ActiveGameState *activeState = (ActiveGameState*)GameManager::getSingleton()->getStateManager()->getAppStateByType(AppStateType::ACTIVE_STATE);
		Player *mainPlayer = (activeState ? activeState->getPlayer() : nullptr);

		vector<Player*> selectingPlayers = getSelectingPlayers();
		bool mainPlayerSelecting = (activeState && find(selectingPlayers.begin(), selectingPlayers.end(), mainPlayer) != selectingPlayers.end());
		int ammount = 0, initAmmount = 0;

		if(deposit){
			ammount = deposit->getAmmount();
			initAmmount = deposit->getInitAmmount();
		}

		destructable->displayStats(ammountForeground, ammountBackground, ammount, initAmmount, mainPlayer == player && mainPlayerSelecting, Vector2(0, -10));
	}

	void Extractor::draw(){
		deposit->decreaseAmmount(drawSpeed);
		lastDrawTime = getTime();
	}
}
