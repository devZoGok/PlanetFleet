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

	NewMapButton::OkButton::OkButton(vb01::Vector3 pos, vb01::Vector2 size, vb01Gui::Textbox *name, vb01Gui::Textbox *sx, vb01Gui::Textbox *sy) : Button(pos, size, "Ok", GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", -1, true){
		this->name = name;
		this->sizeX = sx;
		this->sizeY = sy;
	}

	void NewMapButton::OkButton::onClick(){
		Vector2 size = Vector2(
					atof(wstringToString(sizeX->getText()).c_str()),
					atof(wstringToString(sizeY->getText()).c_str())
				);
		handleLoadingGui(new LoadingAppState(new MapEditorAppState(wstringToString(name->getText()), size, true), "mapEditor.lua"));
	}

	NewMapButton::NewMapButton(Vector3 pos, Vector2 size) : Button(pos, size, "New map", GameManager::getSingleton()->getPath() + "Fonts/batang.ttf"){}

	void NewMapButton::onClick(){
		ConcreteGuiManager::getSingleton()->readLuaScreenScript("newMap.lua");
	}
}

