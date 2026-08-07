#include "newMapButton.h"
#include "gameManager.h"
#include "mapEditorAppState.h"
#include "loadingAppState.h"
#include "concreteGuiManager.h"

#include <stateManager.h>

#include <util.h>
#include <textbox.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;
	using namespace gameBase;

	NewMapButton::OkButton::OkButton(Vector3 pos, Vector2 size, Textbox *nm, Textbox *sx, Textbox *sy) : 
		PfButtonBase(pos, size, "Ok", GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", -1, true), 
		name(nm), 
		sizeX(sx), 
		sizeY(sy)
	{}

	void NewMapButton::OkButton::onClick(){
		Vector2 size = Vector2(
					atof(wstringToString(sizeX->getText()).c_str()),
					atof(wstringToString(sizeY->getText()).c_str())
				);
		handleLoadingGui(new LoadingAppState(new MapEditorAppState(wstringToString(name->getText()), size, true), "mapEditor.lua"));
	}

	NewMapButton::NewMapButton(Vector3 pos, Vector2 size) : PfButtonBase(pos, size, "New map", GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", -1, true){}

	void NewMapButton::onClick(){
		ConcreteGuiManager::getSingleton()->readLuaScreenScript("newMap.lua");
	}
}

