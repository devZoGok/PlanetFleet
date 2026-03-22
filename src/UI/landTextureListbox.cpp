#include "landTextureListbox.h"
#include "mapEditorAppState.h"
#include "gameManager.h"

#include <material.h>
#include <texture.h>
#include <node.h>
#include <mesh.h>

#include <stateManager.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;
	using namespace gameBase;
	
	LandTextureListbox::LandTextureListbox(Vector3 pos, Vector2 size, vector<string> lines, int maxDisplay, std::string fontPath) : Listbox(pos, size, lines, maxDisplay, fontPath){}
	
	void LandTextureListbox::onClose(){
		StateManager *sm = GameManager::getSingleton()->getStateManager();
		MapEditorAppState::MapEditor *mapEditor = ((MapEditorAppState*)sm->getAppStateByType((int)AppStateType::MAP_EDITOR))->getMapEditor();
	
		Map *map = Map::getSingleton();
		Material *mat = map->getNodeParent()->getChild(0)->getMesh(0)->getMaterial();
		Material::BoolUniform *texturingUniform = (Material::BoolUniform*)mat->getUniform("texturingEnabled");
		Texture *landTex = mapEditor->getLandmassTexture(selectedOption);
		string texUni = "textures[0]";
	
		if(!texturingUniform->value){
			texturingUniform->value = true;
			mat->addTexUniform(texUni, landTex, false);
		}
		else
			mat->setTexUniform(texUni, landTex, false);
	}
}
