#include "tradingScreenButton.h"
#include "concreteGuiManager.h"
#include "activeGameState.h"
#include "offerButton.h"
#include "gameManager.h"
#include "tradeOffer.h"
#include "game.h"

#include <listbox.h>
#include <textbox.h>

#include <stateManager.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;
	using namespace gameBase;

	TradingScreenButton::TradingScreenButton(Vector3 pos, Vector2 size, Listbox *lb, int plId, string gs, string name, int trigger, string imagePath) : 
		ActiveStateButton(pos, size, gs, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", trigger, imagePath), 
		listbox(lb),
		playerId(plId)
	{}

	void TradingScreenButton::onClick(){
		generateView().script("playerId = " + to_string(playerId));

		ConcreteGuiManager *guiManager = ConcreteGuiManager::getSingleton();
		Game *game = Game::getSingleton();

		StateManager *stateManager = GameManager::getSingleton()->getStateManager();
		ActiveGameState *activeState = (ActiveGameState*)stateManager->getAppStateByType((int)AppStateType::ACTIVE_STATE);
		activeState->setOfferScreen(true);

		if(listbox->getNumLines() > 0){
			Player *mainPlayer = activeState->getPlayer();
			
			TradeOffer *tradeOffer = mainPlayer->getTradeOffers(game->getPlayer(playerId))[listbox->getSelectedOption()];

			ActiveStateButton::onClick();
			vector<Textbox*> textboxes = guiManager->getTextboxes();
			
			textboxes[0]->setEntry(to_wstring(tradeOffer->buyRefineds));
			textboxes[1]->setEntry(to_wstring(tradeOffer->sellRefineds));
			textboxes[2]->setEntry(to_wstring(tradeOffer->buyWealth));
			textboxes[3]->setEntry(to_wstring(tradeOffer->sellWealth));
			textboxes[4]->setEntry(to_wstring(tradeOffer->buyResearch));
			textboxes[5]->setEntry(to_wstring(tradeOffer->sellResearch));
		}
		else
			ActiveStateButton::onClick();

		vector<Text*> texts = guiManager->getTexts();
		texts[texts.size() - 2]->setText(stringToWstring(game->getPlayer(playerId)->getName()));

		guiManager->removeButton(this);
	}
}
