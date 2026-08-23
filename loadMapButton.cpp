#include "loadMapButton.h"
#include "concreteGuiManager.h"
#include "gameManager.h"
#include "mapEditorAppState.h"
#include "loadingAppState.h"

#include <stateManager.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;
	using namespace gameBase;

	LoadMapButton::OkButton::OkButton(Vector3 pos, Vector2 size, Listbox *lb) : 
		PfButtonBase(pos, size, "Ok", GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", -1, true), 
		listbox(lb)
	{}

	void LoadMapButton::OkButton::onClick(){
		StateManager *sm = GameManager::getSingleton()->getStateManager();
		string name = wstringToString(listbox->getContents()[listbox->getSelectedOption()]);
		handleLoadingGui(new LoadingAppState(new MapEditorAppState(name, Vector2::VEC_ZERO, false), "mapEditor.lua"));
	}

	LoadMapButton::LoadMapButton(Vector3 pos, Vector2 size) : PfButtonBase(pos, size, "Load map", GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", -1, true){}

	void LoadMapButton::onClick(){
		ConcreteGuiManager::getSingleton()->readLuaScreenScript("loadMap.lua");
	}
}
