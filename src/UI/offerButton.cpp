#include "offerButton.h"
#include "concreteGuiManager.h"
#include "activeGameState.h"
#include "gameManager.h"
#include "tradeOffer.h"
#include "player.h"
#include "game.h"

#include <stateManager.h>

#include <textbox.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;
	using namespace gameBase;

	OfferButton::OfferButton(Vector3 pos, Vector2 size, int plId, string name, int trigger, string imagePath) : 
		Button(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", trigger, true, imagePath),
		playerId(plId){}

	void OfferButton::onClick(){
		ConcreteGuiManager *guiManager = ConcreteGuiManager::getSingleton();
		vector<Textbox*> textboxes = guiManager->getTextboxes();

		int buyRef = (textboxes[0]->getText() == L"" ? 0 : stoi(textboxes[0]->getText())); 
		int sellRef = (textboxes[1]->getText() == L"" ? 0 : stoi(textboxes[1]->getText())); 
		int buyWealth = (textboxes[2]->getText() == L"" ? 0 : stoi(textboxes[2]->getText())); 
		int sellWealth = (textboxes[3]->getText() == L"" ? 0 : stoi(textboxes[3]->getText()));
		int buyRes = (textboxes[4]->getText() == L"" ? 0 : stoi(textboxes[4]->getText()));
		int sellRes = (textboxes[5]->getText() == L"" ? 0 : stoi(textboxes[5]->getText())); 

		StateManager *stateManager = GameManager::getSingleton()->getStateManager();
		ActiveGameState *activeState = (ActiveGameState*)stateManager->getAppStateByType((int)AppStateType::ACTIVE_STATE);
		Player *mainPlayer = activeState->getPlayer();

		mainPlayer->addTradeOffer(Game::getSingleton()->getPlayer(playerId), new TradeOffer(buyRef, sellRef, buyWealth, sellWealth, buyRes, sellRes));
	}
}
