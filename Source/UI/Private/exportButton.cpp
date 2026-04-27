#include "exportButton.h"
#include "gameManager.h"
#include "mapEditorAppState.h"

#include <stateManager.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;
	using namespace gameBase;

	ExportButton::ExportButton(Vector3 pos, Vector2 size) : Button(pos, size, "Export", GameManager::getSingleton()->getPath() + "Fonts/batang.ttf"){}

	void ExportButton::onClick(){
		StateManager *sm = GameManager::getSingleton()->getStateManager();
		MapEditorAppState::MapEditor *mapEditor = ((MapEditorAppState*)sm->getAppStateByType((int)AppStateType::MAP_EDITOR))->getMapEditor();
		mapEditor->exportMap();
	}
}
