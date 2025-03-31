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
		Player *player = ((ActiveGameState*)GameManager::getSingleton()->getStateManager()->getAppStateByType(AppStateType::ACTIVE_STATE))->getPlayer();
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
					break;
				}
			}
		}
		else
			for(Unit *unit : units)
				unit->halt();
	}
}
