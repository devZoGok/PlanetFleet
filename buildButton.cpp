#include "buildButton.h"
#include "gameManager.h"
#include "gameObject.h"
#include "unit.h"
#include "activeGameState.h"
#include "buildableUnit.h"
#include "gameObjectFrameController.h"

#include <stateManager.h>
#include <solUtil.h>

namespace battleship{
	using namespace sol;
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;
	using namespace gameBase;

	BuildButton::BuildButton(Vector3 pos, Vector2 size, string name, int trigger, string imagePath, int uid, int slId) : 
		UnitButton(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", trigger, imagePath, uid), slotId(slId){}

	void BuildButton::onClick(){
		ActiveGameState *activeState = (ActiveGameState*)(GameManager::getSingleton()->getStateManager()->getAppStateByType((int)AppStateType::ACTIVE_STATE));
		Player *player = activeState->getPlayer();
		Unit* builder = player->getUnitsById(unitId)[0];

		if(builder->getBuildableUnit(slotId).buildable){
			GameObjectFrameController *ufCtr = GameObjectFrameController::getSingleton();
			ufCtr->addGameObjectFrame(GameObjectFrame(builder->getBuildableUnit(slotId).id, GameObject::Type::UNIT));
			ufCtr->setPlacingOnSurface(true);
			activeState->setBuildableStructSelected(true);
		}
	}
}
