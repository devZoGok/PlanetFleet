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

	PlayButton::PlayButton(Listbox *ml, Vector3 pos, Vector2 size, string name, bool separate) : Button(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", GLFW_KEY_P, separate), mapListbox(ml) {}
	
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

		Game *game = Game::getSingleton();
		game->initTechnologies();

		int selectedMap = mapListbox->getSelectedOption();
		string mapName = wstringToString(mapListbox->getContents()[selectedMap]);
		int numPlayers = Map::getSingleton()->getNumMapSpawnPoints(mapName);
		const int MAIN_PLAYER_ID = (int)generateView()["MAIN_PLAYER_ID"];
	
		for(int i = 0; i < numPlayers; i++){
			int faction = factionsListboxes[i]->getSelectedOption();
			bool cpuPlayer;

			if(factionsListboxes[i]->getContents()[faction] == L"None") continue;

			if(MAIN_PLAYER_ID == i)
				cpuPlayer = false;
			else{
				faction--;
				cpuPlayer = true;
			}

			string diffStr = (cpuPlayer ? wstringToString(difficultiesListboxes[i]->getContents()[colorsListboxes[i]->getSelectedOption()]) : "");
			int difficulty = -1;

			if(diffStr == "Easy") difficulty = 0;
			else if(diffStr == "Medium") difficulty = 1;
			else if(diffStr == "Hard") difficulty = 2;

			string colorStr = wstringToString(colorsListboxes[i]->getContents()[colorsListboxes[i]->getSelectedOption()]);
			Vector3 color;

			if(colorStr == "Black") color = Vector3::VEC_ZERO;
			else if(colorStr == "Red") color = Vector3::VEC_I;
			else if(colorStr == "Green") color = Vector3::VEC_J;
			else if(colorStr == "Blue") color = Vector3::VEC_K;
			else if(colorStr == "White") color = Vector3::VEC_IJK;

			int team = stoi(teamsListboxes[i]->getContents()[teamsListboxes[i]->getSelectedOption()]);

			string name = (cpuPlayer ? "CPU player #" + to_string(i) : "Player");
			game->addPlayer(new Player(difficulty, faction, team, color, cpuPlayer, i, name));
		}
	
		handleLoadingGui(new LoadingAppState(new InGameAppState(mapName), "inGame.lua"));
	}
}
