#include "addResourceCommand.h"
#include "game.h"
#include "player.h"
#include "trader.h"

namespace battleship{
	void AddResourceCommand::validate(){
		if(arguments.size() != 3)
			return;

		playerId = atoi(arguments[0].c_str());

		if(playerId != 0 && playerId != 1)
			return;

		resourceId = atoi(arguments[1].c_str());

		if(!(0 <= resourceId && resourceId <= 2))
			return;

		resourceAmmount = atoi(arguments[2].c_str());
	}

	void AddResourceCommand::execute(){
		AbstractCommand::execute();

		Player *player = Game::getSingleton()->getPlayer(playerId);
		player->updateResource(ResourceType(resourceId), resourceAmmount, true);
	}
}
