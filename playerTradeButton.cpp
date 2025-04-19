#include "playerTradeButton.h"
#include "concreteGuiManager.h"
#include "activeGameState.h"
#include "offerButton.h"
#include "game.h"

#include <stateManager.h>
#include <solUtil.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;
	using namespace gameBase;

	PlayerTradeButton::PlayerTradeButton(Vector3 pos, Vector2 size, string gs, string name, int trigger, string imagePath) : ActiveStateButton(pos, size, gs, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", trigger, imagePath){}

	void PlayerTradeButton::onClick(){
		StateManager *stateManager = GameManager::getSingleton()->getStateManager();
		ActiveGameState *activeState = (ActiveGameState*)stateManager->getAppStateByType((int)AppStateType::ACTIVE_STATE);

		if(activeState->isTradingScreen() && !activeState->isOfferScreen()) return;
		else if(!activeState->isTradingScreen()) activeState->setTradingScreen(true);
		else if(activeState->isOfferScreen()) activeState->setOfferScreen(false);

		ActiveStateButton::onClick();

		Player *mainPlayer = activeState->getPlayer();
		Game *game = Game::getSingleton();

		vector<Player*> players = game->getPlayers();
		ConcreteGuiManager *guiManager = ConcreteGuiManager::getSingleton();
		sol::state_view SOL_LUA_VIEW = generateView();

		for(int i = 0, lineId = 0; i < players.size(); i++){
			if(players[i] == mainPlayer) continue;

			SOL_LUA_VIEW.script("lineId = " + to_string(lineId));
			SOL_LUA_VIEW.script("playerId = " + to_string(i));
			SOL_LUA_VIEW.script(string("relationsIcon = ") + (players[i]->getTeam() == mainPlayer->getTeam() ? "allianceIcon" : "warIcon"));
			guiManager->parseLuaScript("tradingPlayerGuiTray.lua");

			vector<Button*> guiButtons = guiManager->getButtons();

			for(int j = guiButtons.size() - 1; j > 0; j--)
				if(guiButtons[j]->getName() == "Offer")
					((OfferButton*)guiButtons[j])->setPlayerId(i);

			int numListboxes = guiManager->getListboxes().size();
			Listbox *listbox = guiManager->getListboxes()[numListboxes - 1];
			int numOffers = mainPlayer->getTradeOffers(players[i]).size();

			for(int j = 0; j < numOffers; j++)
				listbox->addLine(L"TRADE_OFFER_" + to_wstring(j));

			if(listbox->getNumLines() < listbox->getMaxDisplay())
				listbox->setMaxDisplay(listbox->getNumLines());

			int numTexts = guiManager->getTexts().size();
			guiManager->getTexts()[numTexts - 1]->setText(stringToWstring(players[i]->getName()));

			lineId++;
		}
	}
}
