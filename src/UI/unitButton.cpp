#include "unitButton.h"
#include "gameManager.h"
#include "activeGameState.h"

#include <stateManager.h>

#include <solUtil.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;
	using namespace gameBase;

	UnitButton::UnitButton(Vector3 pos, Vector2 size, string name, string fontPath, int trigger, string imagePath) : Button(pos, size, name, fontPath, trigger, true, imagePath){
		StateManager *stateManager = GameManager::getSingleton()->getStateManager();
		ActiveGameState *activeState = (ActiveGameState*)stateManager->getAppStateByType((int)AppStateType::ACTIVE_STATE);

		if(activeState) activeState->addButton(this);
	}

	//TODO optimize this code
	vector<Unit*> UnitButton::getUnits(){
		vector<Unit*> units;

		ActiveGameState *activeState = (ActiveGameState*)(GameManager::getSingleton()->getStateManager()->getAppStateByType((int)AppStateType::ACTIVE_STATE));
		Player *player = activeState->getPlayer();

		sol::state_view SOL_LUA_VIEW = generateView();
		int unitId = SOL_LUA_VIEW["_mainUnitId"];
		vector<Unit*> selUnits = player->getSelectedUnits(), researchStructs = player->getUnitsById(unitId);

		for(Unit *rs : researchStructs)
			if(find(selUnits.begin(), selUnits.end(), rs) != selUnits.end())
				units.push_back(rs);

		return units;
	}
}
