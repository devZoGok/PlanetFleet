#include "tradeButton.h"
#include "activeGameState.h"
#include "player.h"

#include <stateManager.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace gameBase;

	TradeButton::TradeButton(Vector3 pos, Vector2 size, string name, int trigger, string imagePath, Type t) : UnitButton(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", trigger, imagePath), type(t){}

	void TradeButton::onClick(){
		ActiveGameState *activeState = (ActiveGameState*)(GameManager::getSingleton()->getStateManager()->getAppStateByType((int)AppStateType::ACTIVE_STATE));
		Player *player = activeState->getPlayer();

		int resType, ammount = 10;
		bool buy;

		switch(type){
			case Type::BUY_REFINEDS:
				resType = (int)ResourceType::REFINEDS;
				buy = true;
				break;
			case Type::SELL_REFINEDS:
				resType = (int)ResourceType::REFINEDS;
				buy = false;
				break;
			case Type::BUY_RESEARCH:
				resType = (int)ResourceType::RESEARCH;
				buy = true;
				break;
			case Type::SELL_RESEARCH:
				resType = (int)ResourceType::RESEARCH;
				buy = false;
				break;
		}

		player->getTrader()->trade(player, resType, ammount, buy);
	}
}
