#include "activeStateBackButton.h"
#include "activeGameState.h"
#include "gameManager.h"

#include <stateManager.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace gameBase;

	ActiveStateBackButton::ActiveStateBackButton(Vector3 pos, Vector2 size, string name) : BackButton(pos, size, name, "activeGameState.lua"){}

	void ActiveStateBackButton::onClick(){
		StateManager *stateManager = GameManager::getSingleton()->getStateManager();
		ActiveGameState *activeState = (ActiveGameState*)stateManager->getAppStateByType((int)AppStateType::ACTIVE_STATE);
		activeState->setTradingScreen(false);

		BackButton::onClick();
	}
}
