#include "game.h"
#include "player.h"
#include "projectile.h"
#include "gameManager.h"
#include "activeGameState.h"
#include "inGameAppState.h"
#include "concreteGuiManager.h"
#include "fxManager.h"
#include "factory.h"
#include "defConfigs.h"

#include <algorithm>

#include <assetManager.h>
#include <particleEmitter.h>

#include <stateManager.h>

#include <solUtil.h>

#include <SFML/Audio.hpp>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace gameBase;

	static Game *game = nullptr;

	Game* Game::getSingleton(){
		if(!game)
			game = new Game();

		return game;
	}

	void Game::endGame(bool victory){
		ended = true;

		StateManager *sm = GameManager::getSingleton()->getStateManager();
		sm->dettachAppState(sm->getAppStateByType(AppStateType::ACTIVE_STATE));

		ConcreteGuiManager::getSingleton()->parseLuaScript(victory ? "victory.lua" : "defeat.lua");
	}

	void Game::update(){
		sol::state_view SOL_LUA_VIEW = generateView();

		for(int i = 0; i < getCpuPlayers().size(); i++) {
			string plStr = "game.cpuPlayers[" + to_string(i + 1) + "]";
			SOL_LUA_VIEW.script("executeBtNode(" + plStr + ", " + plStr + ".behaviour)");
		}

		int numPlayersWithUnits = 0;

		for(int i = 0; i < players.size(); i++){
			players[i]->update();

			if(players[i]->getNumUnits() > 0)
				numPlayersWithUnits++;
		}

		ActiveGameState *activeState = (ActiveGameState*)GameManager::getSingleton()->getStateManager()->getAppStateByType(AppStateType::ACTIVE_STATE);
		Player *mainPlayer = (activeState ? activeState->getPlayer() : nullptr);

		if(mainPlayer && !ended){
			int numMainPlayerUnits = mainPlayer->getNumUnits();

			if(numMainPlayerUnits > 0 && numPlayersWithUnits == 1)
				endGame(true);
			else if(numMainPlayerUnits == 0)
				endGame(false);
		}

		FxManager::getSingleton()->update();
	}

	void Game::initLuaPlayers(bool resetBehaviour){
		sol::state_view SOL_LUA_VIEW = generateView();
		vector<Player*> cpuPlayers = getCpuPlayers();

		for(int i = 0; i < cpuPlayers.size(); i++)
			SOL_LUA_VIEW["game"]["cpuPlayers"][i + 1] = cpuPlayers[i];

		if(resetBehaviour)
			SOL_LUA_VIEW.script_file(GameManager::getSingleton()->getPath() + "Scripts/Core/playerInit.lua");
	}

	void Game::removeAllElements(){
		generateView().script("game.cpuPlayers = {}");

		while(!players.empty()){
			delete players[0];
			players.erase(players.begin());
		}

		ended = false;
		paused = false;
	}

	void Game::togglePause(){
		GameManager *gm = GameManager::getSingleton();
		ConcreteGuiManager *guiManager = ConcreteGuiManager::getSingleton();
		ActiveGameState *activeState = ((InGameAppState*)gm->getStateManager()->getAppStateByType(AppStateType::IN_GAME_STATE))->getActiveState();

        if (!paused) {
            gm->getStateManager()->dettachAppState(activeState);
			guiManager->parseLuaScript("gamePaused.lua");
        } 
        else {
			sol::state_view SOL_LUA_VIEW = generateView();

			for(string f : configData::scripts)
				SOL_LUA_VIEW.script_file(gm->getPath() + f);

			initLuaPlayers();
			guiManager->readLuaScreenScript("inGame.lua", activeState->getGuiButtons());

			string gop = SOL_LUA_VIEW["gameObjPrefix"], vfxp = SOL_LUA_VIEW["vfxPrefix"];
			AssetManager::getSingleton()->load(gm->getPath() + gop, true);
			AssetManager::getSingleton()->load(gm->getPath() + vfxp, true);

			if(debug){
				vector<GameObject*> gameObjs;

				for(Player *pl : Game::getSingleton()->getPlayers()){
					for(Unit *u : pl->getUnits())
						gameObjs.push_back((GameObject*)u);

					for(Projectile *proj : pl->getProjectiles())
						gameObjs.push_back((GameObject*)proj);
				}

				for(ResourceDeposit *dep : civilianPlayer->getResourceDeposits())
					gameObjs.push_back((GameObject*)dep);

				for(GameObject *obj : gameObjs)
					obj->reinit();
			}

            gm->getStateManager()->attachAppState(activeState);
        }

		paused = !paused;
	}

	vector<int> Game::parseTechTable(int tid, string key, string numVarKey, string varKey){
		sol::state_view SOL_LUA_VIEW = generateView();
		SOL_LUA_VIEW.script(numVarKey + " = #" + key + "[" + to_string(tid + 1) + "]." + varKey);
		int numVar = SOL_LUA_VIEW[numVarKey];
		sol::table techTable = SOL_LUA_VIEW[key][tid + 1];

		vector<int> varVec;

		for(int i = 0; i < numVar; i++)
			varVec.push_back(techTable[varKey][i + 1]);

		return varVec;
	}

	//TODO clean this method up
	void Game::initTechnologies(){
		technologies.clear();

		sol::state_view SOL_LUA_VIEW = generateView();
		string techKey = "technologies";
		SOL_LUA_VIEW.script("numTechs = #" + techKey);
		int numTechs = SOL_LUA_VIEW["numTechs"]; 

		for(int i = 0; i < numTechs; i++){
			sol::table techTable = SOL_LUA_VIEW[techKey][i + 1];

			Technology t;
			t.cost = techTable["cost"];
			t.name = techTable["name"];
			t.icon = techTable["icon"];
			t.description = techTable["description"];
			t.parents = parseTechTable(i, techKey, "numParents", "parents");
			t.abilities = parseTechTable(i, techKey, "numAbilities", "abilities");

			technologies.push_back(t);
		}

		techKey = "abilities";
		SOL_LUA_VIEW.script("numAbilities = #" + techKey);
		int numAbilities = SOL_LUA_VIEW["numAbilities"]; 

		for(int i = 0; i < numAbilities; i++){
			sol::table techTable = SOL_LUA_VIEW[techKey][i + 1];

			Ability ability;
			ability.type = techTable["type"];
			ability.ammount = techTable["ammount"].get_or(0.0);
			ability.gameObjType = techTable["gameObjType"];
			ability.gameObjIds = parseTechTable(i, techKey, "numGameObjIds", "gameObjIds");
			abilities.push_back(ability);
		}
	}

	float Game::calcAbilFromTech(Ability::Type type, vector<int> techResearch, int gameObjType, int unitId){
		float ammount = 0;

		for(int techId : techResearch)
			for(int abilId : technologies[techId].abilities)
				if(
					abilities[abilId].type == type &&
					abilities[abilId].gameObjType == gameObjType && 
					find(abilities[abilId].gameObjIds.begin(), abilities[abilId].gameObjIds.end(), unitId) != abilities[abilId].gameObjIds.end()
				){
					ammount += abilities[abilId].ammount;
				}

		return ammount;
	}

	bool Game::isUnitUnlocked(vector<int> techResearch, int unitId){
		for(int techId : techResearch){
			for(int abilId : technologies[techId].abilities){
				vector<int> ids = abilities[abilId].gameObjIds;

				if(find(ids.begin(), ids.end(), unitId) != ids.end())
					return true;
			}
		}

		return false;
	}
	
	vector<Player*> Game::getPlayers(bool civPl){
		vector<Player*> playersVec = players;

		if(civPl) playersVec.push_back(civilianPlayer);

		return playersVec;
	}

	vector<Player*> Game::getCpuPlayers(){
		vector<Player*> playersVec;

		for(Player *pl : players)
			if(pl->isCpuPlayer())
				playersVec.push_back(pl);

		return playersVec;
	}
}
