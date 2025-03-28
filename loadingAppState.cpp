#include "loadingAppState.h"
#include "concreteGuiManager.h"
#include "gameManager.h"
#include "defConfigs.h"

#include <quad.h>
#include <node.h>
#include <vector.h>
#include <assetManager.h>

#include <stateManager.h>

namespace battleship{
	using namespace configData;
	using namespace gameBase;
	using namespace vb01;
	using namespace std;

	LoadingAppState::LoadingAppState(AbstractAppState *newSt, string newScr) : AbstractAppState(
		AppStateType::LOADING_STATE,
		configData::calcSumBinds(AppStateType::LOADING_STATE, true),
		configData::calcSumBinds(AppStateType::LOADING_STATE, false),
		GameManager::getSingleton()->getPath() + scripts[(int)ScriptFiles::OPTIONS]
	),
	newState(newSt),
	newScreen(newScr)
	{}

	void LoadingAppState::onDettached(){
		GameManager::getSingleton()->getStateManager()->attachAppState(newState);
		ConcreteGuiManager::getSingleton()->readLuaScreenScript(newScreen);

        AbstractAppState::onDettached();
	}

	void LoadingAppState::update(){
		Node *loadingBar = ConcreteGuiManager::getSingleton()->getGuiRectangle("_loadingBar");
		Quad *quad = (Quad*)loadingBar->getMesh(0);
		quad->setSize(Vector3((1 - (float)loadableAssets.size() / initNumAssets) * 200.f, 20, 0));
		quad->updateVerts(quad->getMeshBase());

		if(getTime() - lastUpdateTime > 5){
			AssetManager::getSingleton()->load(loadableAssets[0]);
			loadableAssets.erase(loadableAssets.begin());

			if(loadableAssets.empty()) GameManager::getSingleton()->getStateManager()->dettachAppState(this);

			lastUpdateTime = getTime();
		}
	}
}
