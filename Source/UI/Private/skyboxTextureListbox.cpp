#include "skyboxTextureListbox.h"
#include "gameManager.h"
#include "mapEditorAppState.h"

#include <stateManager.h>

#include <box.h>
#include <root.h>
#include <texture.h>
#include <material.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;
	using namespace gameBase;

	SkyboxTextureListbox::SkyboxTextureListbox(Vector3 pos, Vector2 size, vector<string> lines, int maxDisplay, string fontPath) : Listbox(pos, size, lines, maxDisplay, fontPath){}
	
	void SkyboxTextureListbox::onClose(){
		Root *root = Root::getSingleton();
		StateManager *sm = GameManager::getSingleton()->getStateManager();
		MapEditorAppState::MapEditor *mapEditor = ((MapEditorAppState*)sm->getAppStateByType((int)AppStateType::MAP_EDITOR))->getMapEditor();
		Texture *skyTexture = mapEditor->getSkyTexture(selectedOption);
		
		if(!root->getSkybox())
			root->createSkybox(skyTexture);
		else
			 root->getSkybox()->getMaterial()->setTexUniform("tex", skyTexture, true);
	}
}
