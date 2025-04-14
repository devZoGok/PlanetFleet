#include "activeStateButton.h"
#include "gameManager.h"
#include "activeGameState.h"
#include "concreteGuiManager.h"

#include <listbox.h>
#include <checkbox.h>
#include <slider.h>
#include <textbox.h>

#include <text.h>

#include <stateManager.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;
	using namespace gameBase;

	ActiveStateButton::ActiveStateButton(Vector3 pos, Vector2 size, string gs, string name, string fontPath, int trigger, string imagePath) : Button(pos, size, name, fontPath, trigger, true, imagePath), guiScreen(gs){
		StateManager *stateManager = GameManager::getSingleton()->getStateManager();
		ActiveGameState *activeState = (ActiveGameState*)stateManager->getAppStateByType((int)AppStateType::ACTIVE_STATE);

		if(activeState)
			activeState->addButton(this);
	}

	void ActiveStateButton::onClick(){
		ConcreteGuiManager::getSingleton()->parseLuaScript(guiScreen);
	}
}
