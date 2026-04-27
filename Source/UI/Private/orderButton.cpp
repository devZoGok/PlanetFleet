#include "orderButton.h"
#include "gameManager.h"
#include "activeGameState.h"
#include "player.h"
#include "unit.h"

#include <stateManager.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;
	using namespace gameBase;

	OrderButton::OrderButton(Vector3 pos, Vector2 size, string name, int trigger, string imagePath, int oid) :
	   	Button(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", trigger, true, imagePath),
		orderId(oid)
	{}

	void OrderButton::onClick(){
		ActiveGameState *activeState = ((ActiveGameState*)GameManager::getSingleton()->getStateManager()->getAppStateByType(AppStateType::ACTIVE_STATE));
		Player *player = activeState->getPlayer();
		vector<Unit*> units = player->getSelectedUnits();

		if(orderId > -1){
			switch(Order::TYPE(orderId)){
				case Order::TYPE::EJECT:
				{
					for(Unit *unit : units)
						unit->receiveOrder(Order(Order::TYPE::EJECT, vector<Order::Target>{}), false);

					break;
				}
				default:
				{
					ActiveGameState::CursorState cs;

					switch((Order::TYPE)orderId){
						case Order::TYPE::ATTACK:
							cs = ActiveGameState::CursorState::ATTACK;
							break;
						case Order::TYPE::GARRISON:
							cs = ActiveGameState::CursorState::GARRISON;
							break;
						case Order::TYPE::SUPPLY:
							cs = ActiveGameState::CursorState::SUPPLY;
							break;
						case Order::TYPE::LOAD:
							cs = ActiveGameState::CursorState::LOAD;
							break;
						case Order::TYPE::UNLOAD:
							cs = ActiveGameState::CursorState::UNLOAD;
							break;
						case Order::TYPE::HACK:
							cs = ActiveGameState::CursorState::HACK;
							break;
					}

					activeState->setCursorState(cs);
					activeState->setForceCursorState(true);
					break;
				}
			}
		}
		else
			for(Unit *unit : units)
				unit->halt();
	}
}
