#include "mapEditorButton.h"
#include "concreteGuiManager.h"

namespace battleship{
	void MapEditorButton::onClick(){
		ConcreteGuiManager::getSingleton()->readLuaScreenScript("mapEditorMenu.lua");
	}
}
