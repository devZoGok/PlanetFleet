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
	// Get the executable's name as the game path
	string gamePath = string(argv[0]);

	for(int i = 0; i < gamePath.length(); i++)
		if(gamePath[i] == '\\')
			gamePath[i] = '/';

	gamePath = gamePath.substr(0, gamePath.find_last_of("/") + 1) + "../";
	// Start the game manager and pass the game path to it
    GameManager *gm = GameManager::getSingleton();
	gm->start(gamePath);
	// Get the state manager and attach a new Gui app state
    gm->getStateManager()->attachAppState(new GuiAppState());
	AssetManager::getSingleton()->load(gm->getPath() + "Fonts/batang.ttf");
	// Begin on the main menu GUI
	ConcreteGuiManager::getSingleton()->readLuaScreenScript("mainMenu.lua");

	// As long as the game manager is running, update it
	while(gm->isRunning()){
		gm->update();
	}

    return 0;
}
