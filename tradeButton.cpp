#include "tradeButton.h"
#include "activeGameState.h"
#include "player.h"

#include <quad.h>
#include <node.h>
#include <text.h>

#include <stateManager.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace gameBase;

	TradeButton::TradeButton(Vector3 pos, Vector2 size, string name, int trigger, string imagePath, Type t, int am) : 
		UnitButton(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", trigger, imagePath), 
		type(t), 
		amount(am)
	{}

	void TradeButton::onClick(){
		ActiveGameState *activeState = (ActiveGameState*)(GameManager::getSingleton()->getStateManager()->getAppStateByType((int)AppStateType::ACTIVE_STATE));
		Player *player = activeState->getPlayer();

		int resType;
		bool buy;
		editResourceTradeData(resType, buy);
		player->getTrader()->trade(player, resType, amount, buy);
	}

	void TradeButton::update(){
		PfButtonBase::update();

		Text *descText = tooltip->getBackground()->getNode()->getChild(0)->getText(0);

		if(tooltipInitDesc == L"") tooltipInitDesc = descText->getText();

		wstring entry = tooltipInitDesc;
		entry.replace(entry.find_first_of(L"_"), 1, to_wstring(amount));

		int rt;
		bool b;
		editResourceTradeData(rt, b);
		ActiveGameState *activeState = (ActiveGameState*)(GameManager::getSingleton()->getStateManager()->getAppStateByType((int)AppStateType::ACTIVE_STATE));
		entry.replace(entry.find_last_of(L"_"), 1, to_wstring(activeState->getPlayer()->getTrader()->getResourceRate(rt)));

		descText->setText(entry);
	}

	void TradeButton::editResourceTradeData(int &resType, bool &buy){
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
	}
}
