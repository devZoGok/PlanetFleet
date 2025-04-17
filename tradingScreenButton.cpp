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
		ConcreteGuiManager *guiManager = ConcreteGuiManager::getSingleton();
		Game *game = Game::getSingleton();

		StateManager *stateManager = GameManager::getSingleton()->getStateManager();
		ActiveGameState *activeState = (ActiveGameState*)stateManager->getAppStateByType((int)AppStateType::ACTIVE_STATE);
		activeState->setOfferScreen(true);

		if(listbox->getNumLines() > 0){
			Player *mainPlayer = activeState->getPlayer();
			
			TradeOffer *tradeOffer = game->findTradeOffers(mainPlayer, game->getPlayer(playerId))[listbox->getSelectedOption()];

			ActiveStateButton::onClick();
			vector<Textbox*> textboxes = guiManager->getTextboxes();
			
			textboxes[0]->setEntry(to_wstring(tradeOffer->recRefineds));
			textboxes[1]->setEntry(to_wstring(tradeOffer->initRefineds));
			textboxes[2]->setEntry(to_wstring(tradeOffer->recWealth));
			textboxes[3]->setEntry(to_wstring(tradeOffer->initWealth));
			textboxes[4]->setEntry(to_wstring(tradeOffer->recResearch));
			textboxes[5]->setEntry(to_wstring(tradeOffer->initResearch));
		}
		else
			ActiveStateButton::onClick();

		vector<Text*> texts = guiManager->getTexts();
		texts[texts.size() - 2]->setText(stringToWstring(game->getPlayer(playerId)->getName()));

		guiManager->removeButton(this);
	}
}
