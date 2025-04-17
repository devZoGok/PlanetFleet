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

	ActiveStateButton::ActiveStateButton(Vector3 pos, Vector2 size, string gs, string name, string fontPath, int trigger, string imagePath) : 
		Button(pos, size, name, fontPath, trigger, true, imagePath), guiScreen(gs){}

	void ActiveStateButton::onClick(){
		StateManager *stateManager = GameManager::getSingleton()->getStateManager();
		ActiveGameState *activeState = (ActiveGameState*)stateManager->getAppStateByType((int)AppStateType::ACTIVE_STATE);

		vector<Button*> buttons = activeState->getGuiButtons();
		buttons.push_back(this);

		vector<Node*> rects = activeState->getGuiRects();
		vector<Text*> texts = activeState->getGuiTexts();

		ConcreteGuiManager::getSingleton()->readLuaScreenScript(
			guiScreen,
			buttons,
			vector<Listbox*>{},
			vector<Checkbox*>{},
			vector<Slider*>{},
			vector<Textbox*>{},
			rects,
			texts
		);
	}
}
