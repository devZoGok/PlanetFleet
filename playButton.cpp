#include "playButton.h"
#include "gameManager.h"
#include "inGameAppState.h"
#include "loadingAppState.h"
#include "concreteGuiManager.h"
#include "game.h"

#include <stateManager.h>

#include <glfw3.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;
	using namespace gameBase;

	PlayButton::PlayButton(Listbox *ml, Vector3 pos, Vector2 size, string name, bool separate) : PfButtonBase(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", GLFW_KEY_P, separate), mapListbox(ml) {}
	
	// Starts the gameplay
	void PlayButton::onClick() {
		ConcreteGuiManager *guiManager = ConcreteGuiManager::getSingleton();
		vector<Listbox*> factionsListboxes, difficultiesListboxes, colorsListboxes, teamsListboxes;

		for(Listbox *listbox : guiManager->getListboxes()){
			string name = listbox->getName();

			if(name == "factions") factionsListboxes.push_back(listbox);
			else if(name == "difficulties") difficultiesListboxes.push_back(listbox);
			else if(name == "colors") colorsListboxes.push_back(listbox);
			else if(name == "teams") teamsListboxes.push_back(listbox);
		}

		// Initialize the game
		Game *game = Game::getSingleton();
		game->initTechnologies();

		int selectedMap = mapListbox->getSelectedOption();
		string mapName = wstringToString(mapListbox->getContents()[selectedMap]);
		int numPlayers = Map::getSingleton()->getNumMapSpawnPoints(mapName);
		const int MAIN_PLAYER_ID = (int)generateView()["MAIN_PLAYER_ID"];
	
		for(int i = 0; i < numPlayers; i++){
			if(i != MAIN_PLAYER_ID && difficultiesListboxes[i - 1]->getContents()[difficultiesListboxes[i - 1]->getSelectedOption()] == L"None") continue;

			wstring factionStr = factionsListboxes[i]->getContents()[factionsListboxes[i]->getSelectedOption()];
			int faction = (factionStr == L"Random" ? rand() % 3 : factionsListboxes[i]->getSelectedOption() - 1);

			bool cpuPlayer = (MAIN_PLAYER_ID != i);
			int difficulty = (cpuPlayer ? difficultiesListboxes[i - 1]->getSelectedOption() : -1);

			Vector4 color = ((Material::Vector4Uniform*)colorsListboxes[i]->getLineText(colorsListboxes[i]->getSelectedOption())->getMaterial()->getUniform("diffuseColor"))->value;
			int team = stoi(teamsListboxes[i]->getContents()[teamsListboxes[i]->getSelectedOption()]);
			string name = (cpuPlayer ? "CPU player #" + to_string(i) : "Player");

			game->addPlayer(new Player(difficulty, faction, team, Vector3(color.x, color.y, color.z), cpuPlayer, i, name));
		}
		
		// HUD GUI for during gameplay
		// The loading app state passed is an InGameAppState for the selected map with the inGame.lua script for the screen
		// The initial state of the current map is used to populate the HUD GUI here
		handleLoadingGui(new LoadingAppState(new InGameAppState(mapName), "inGame.lua"));
	}
}
