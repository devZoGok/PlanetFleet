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

	LoadMapButton::OkButton::OkButton(vb01::Vector3 pos, vb01::Vector2 size, vb01Gui::Listbox *listbox) : Button(pos, size, "Ok", GameManager::getSingleton()->getPath() + "Fonts/batang.ttf"){ this->listbox = listbox; }

	void LoadMapButton::OkButton::onClick(){
		StateManager *sm = GameManager::getSingleton()->getStateManager();
		string name = wstringToString(listbox->getContents()[listbox->getSelectedOption()]);
		handleLoadingGui(new LoadingAppState(new MapEditorAppState(name, Vector2::VEC_ZERO, false), "mapEditor.lua"));
	}

	LoadMapButton::LoadMapButton(Vector3 pos, Vector2 size) : Button(pos, size, "Load map", GameManager::getSingleton()->getPath() + "Fonts/batang.ttf"){}

	void LoadMapButton::onClick(){
		ConcreteGuiManager::getSingleton()->readLuaScreenScript("loadMap.lua");
	}
}
