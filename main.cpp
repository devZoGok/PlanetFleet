#include "util.h"
#include "gameManager.h"
#include "guiAppState.h"
#include "concreteGuiManager.h"

#include <stateManager.h>

#include <assetManager.h>
#include <root.h>

#include <string>

using namespace battleship;
using namespace vb01;
using namespace std;

int main(int argc, char **argv) {
	string gamePath = string(argv[0]);

	for(int i = 0; i < gamePath.length(); i++)
		if(gamePath[i] == '\\')
			gamePath[i] = '/';

	gamePath = gamePath.substr(0, gamePath.find_last_of("/") + 1) + "../";

    GameManager *gm = GameManager::getSingleton();
	gm->start(gamePath);
    gm->getStateManager()->attachAppState(new GuiAppState());
	AssetManager::getSingleton()->load(gm->getPath() + "Fonts/batang.ttf");

	ConcreteGuiManager::getSingleton()->readLuaScreenScript("mainMenu.lua");

	while(gm->isRunning()){
		gm->update();
	}

    return 0;
}
