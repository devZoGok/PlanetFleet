#include <algorithm>

#include <button.h>

#include <util.h>
#include <root.h>
#include <camera.h>
#include <assetManager.h>

#include <stateManager.h>

#include "defConfigs.h"
#include "inGameAppState.h"
#include "game.h"
#include "console.h"
#include "gameObjectFrameController.h"
#include "concreteGuiManager.h"
#include "vessel.h"

using namespace vb01;
using namespace vb01Gui;
using namespace std;

namespace battleship{
	using namespace configData;

    InGameAppState::ResumeButton::ResumeButton(Vector3 pos, Vector2 size) : Button(pos, size, "Resume", GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", -1, true) {}

    void InGameAppState::ResumeButton::onClick() {
		Game::getSingleton()->togglePause();
    }

    InGameAppState::ConsoleButton::ConsoleButton(Vector3 pos, Vector2 size) : Button(pos, size, "Console", GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", -1, true) { }

    void InGameAppState::ConsoleButton::onClick() {
		ConcreteGuiManager::getSingleton()->readLuaScreenScript("console.lua");
    }

	InGameAppState::ConsoleButton::ConsoleButton::ConsoleCommandEntryButton::ConsoleCommandEntryButton(Textbox *t, Listbox *l, Vector3 pos, Vector2 size, string name) : Button(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", 257, true) {
	    textbox = t;
	    listbox = l;
	}
	
	void InGameAppState::ConsoleButton::ConsoleButton::ConsoleCommandEntryButton::onClick() {
		Console::execute(wstringToString(textbox->getText()));
	}

    InGameAppState::InGameAppState(vector<string> difficultyLevels, vector<string> factions) : AbstractAppState(
						AppStateType::IN_GAME_STATE,
					 	configData::calcSumBinds(AppStateType::IN_GAME_STATE, true),
					 	configData::calcSumBinds(AppStateType::IN_GAME_STATE, false),
					 	GameManager::getSingleton()->getPath() + scripts[(int)ScriptFiles::OPTIONS]){
        this->playerId = 1;
        this->difficultyLevels = difficultyLevels;
        this->factions = factions;
    }

    InGameAppState::~InGameAppState() {
    }

    void InGameAppState::onAttached() {
        AbstractAppState::onAttached();

        for (int i = 0; i < factions.size(); i++) {
            int faction, difficulty;

            if (i == 0)
                difficulty = -1;
            else {
                if (difficultyLevels[i - 1] == "Easy")
                    difficulty = 0;
                else if (difficultyLevels[i - 1] == "Medium")
                    difficulty = 1;
                else
                    difficulty = 2;
            }

            faction = factions[i][0] - 48;
        }

		Camera *cam = Root::getSingleton()->getCamera();
		cam->setPosition(Map::getSingleton()->getSpawnPoint(playerId - 1) + Vector3(1, 1, 1) * configData::CAMERA_DISTANCE);
		cam->lookAt(Vector3(0, -1, -1).norm(), Vector3(0, 1, -1).norm());

        mainPlayer = Game::getSingleton()->getPlayer(playerId);

		GameManager *gm = GameManager::getSingleton();
		StateManager *stateManager = gm->getStateManager();
        GuiAppState *guiState = ((GuiAppState*)stateManager->getAppStateByType((int)AppStateType::GUI_STATE));
        activeState = new ActiveGameState(guiState, playerId);
        stateManager->attachAppState(activeState);
    }

    void InGameAppState::onDettached() {
		StateManager *sm = GameManager::getSingleton()->getStateManager();
		ActiveGameState *activeState = (ActiveGameState*)sm->getAppStateByType(int(AppStateType::ACTIVE_STATE));
		sm->dettachAppState(activeState);
		delete activeState;
	}

    void InGameAppState::update() {
		Game::getSingleton()->update();
		Map::getSingleton()->update();
    }

    void InGameAppState::onAction(int bind, bool isPressed) {
        switch((Bind)bind){
			case Bind::TOGGLE_MAIN_MENU: 
            	if(isPressed && !GameObjectFrameController::getSingleton()->isPlacingOnSurface())
					Game::getSingleton()->togglePause();
            break;
        }
    }

    void InGameAppState::onAnalog(int bind, float str) {}
}
