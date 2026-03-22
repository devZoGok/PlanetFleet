#include "stateToggleButton.h"
#include "gameManager.h"
#include "activeGameState.h"
#include "player.h"
#include "unit.h"

#include <stateManager.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;
	using namespace gameBase;

	StateToggleButton::StateToggleButton(Vector3 pos, Vector2 size, string name, int trigger, string imagePath) : Button(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", trigger, true, imagePath) {
		updateStateId();
		toggleImage();
	}

	void StateToggleButton::updateStateId(){
		const int NUM_STATES = 3;
		int numUnitsByState[NUM_STATES]{0, 0, 0};

		Player *player = ((ActiveGameState*)GameManager::getSingleton()->getStateManager()->getAppStateByType(AppStateType::ACTIVE_STATE))->getPlayer();
		vector<Unit*> units = player->getSelectedUnits();

		for(Unit *unit : units)
			numUnitsByState[(int)unit->getState()]++;

		int maxId = 0, maxNum = numUnitsByState[maxId];

		for(int i = 0; i < NUM_STATES; i++)
			if(maxNum < numUnitsByState[i]){
				maxId = i;
				maxNum = numUnitsByState[maxId];
			}

		currStateId = maxId;
	}

	void StateToggleButton::toggleImage(){
		string imgPath = "";

		switch(Unit::State(currStateId)){
			case Unit::State::HOLD_FIRE:
				imgPath = "holdFire.png";
				break;
			case Unit::State::CHASE:
				imgPath = "chase.png";
				break;
			case Unit::State::STAND_GROUND:
				imgPath = "standGround.png";
				break;
		}

		setImage(GameManager::getSingleton()->getPath() + "Textures/Icons/Buttons/" + imgPath);
	}

	void StateToggleButton::onClick(){
		const int NUM_STATES = 3;
		Unit::State nextState = (currStateId + 1 == NUM_STATES ? Unit::State(0) : Unit::State(currStateId + 1));

		Player *player = ((ActiveGameState*)GameManager::getSingleton()->getStateManager()->getAppStateByType(AppStateType::ACTIVE_STATE))->getPlayer();
		vector<Unit*> units = player->getSelectedUnits();

		for(Unit *unit : units) unit->setState(nextState);

		updateStateId();
		toggleImage();
	}
}
