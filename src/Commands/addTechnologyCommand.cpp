#include "addTechnologyCommand.h"
#include "game.h"
#include "player.h"

#include <solUtil.h>

#include <algorithm>

namespace battleship
{
	using namespace std;
	using namespace gameBase;

	void AddTechnologyCommand::validate()
	{
		if (arguments.size() != 2)
			return;

		playerId = atoi(arguments[0].c_str());
		int numPlayers = Game::getSingleton()->getNumPlayers();

		if (!(0 <= playerId && playerId < numPlayers))
			return;

		sol::state_view SOL_LUA_VIEW = generateView();
		SOL_LUA_VIEW.script("numTechs = #technologies");
		int numTechs = SOL_LUA_VIEW["numTechs"];

		techId = atoi(arguments[1].c_str());

		if (!(0 <= techId && techId < numTechs))
			return;
	}

	void AddTechnologyCommand::execute()
	{
		AbstractCommand::execute();
		Game::getSingleton()->getPlayer(playerId)->addTechnology(techId);
	}
}
