#include "toggleDebugCommand.h"
#include "game.h"

namespace battleship{
	void ToggleDebugCommand::validate(){
		if(!arguments.empty()) return;
	}

	void ToggleDebugCommand::execute(){
		AbstractCommand::handle();
		validate();

		Game *game = Game::getSingleton();
		game->setDebug(!game->isDebug());
	}
}
