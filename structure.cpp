#include "structure.h"
#include "activeGameState.h"
#include "destructable.h"

#include <stateManager.h>

#include <node.h>

using namespace gameBase;
using namespace vb01;
using namespace std;

namespace battleship{
	Structure::Structure(Player *player, int id, Vector3 pos, Quaternion rot, int bldSt, Unit::State state) : Unit(player, id, pos, rot, state), buildStatus(bldSt){
		Vector2 size = Vector2(lenHpBar, 10);
		buildStatusBackground = destructable->createBar(Vector2::VEC_ZERO, size,  Vector4(0, 0, 0, 1));
		buildStatusForeground = destructable->createBar(Vector2::VEC_ZERO, size,  Vector4(0, 0, 1, 1));
	}

	Structure::~Structure(){
		destructable->removeBar(buildStatusForeground);
		destructable->removeBar(buildStatusBackground);
	}

	void Structure::update(){
		Unit::update();

		ActiveGameState *activeState = (ActiveGameState*)GameManager::getSingleton()->getStateManager()->getAppStateByType(AppStateType::ACTIVE_STATE);
		Player *mainPlayer = (activeState ? activeState->getPlayer() : nullptr);

		vector<Player*> selectingPlayers = getSelectingPlayers();
		bool mainPlayerSelecting = (activeState && find(selectingPlayers.begin(), selectingPlayers.end(), mainPlayer) != selectingPlayers.end());

		if(buildStatus < 100)
			destructable->displayStats(buildStatusForeground, buildStatusBackground, buildStatus, 100, mainPlayer == player && mainPlayerSelecting, Vector2(0, -10));
		else{
			buildStatusBackground->setVisible(false);
			buildStatusForeground->setVisible(false);
		}
	}
}
