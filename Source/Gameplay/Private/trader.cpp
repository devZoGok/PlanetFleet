#include "trader.h"
#include "player.h"

#include <algorithm>

#include <solUtil.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace gameBase;

	Trader::Trader(){
		initProperties();
	}

	void Trader::initProperties(){
		sol::table traderTable = generateView()["trading"];
		fluctuationRate = traderTable["fluctuationRate"];
		fluctuationAmmount = traderTable["fluctuationAmmount"];
		refinedsRate = traderTable["refinedsRate"];
		researchRate = traderTable["researchRate"];
	}

	void Trader::update(){
		if(getTime() - lastUpdateTime > fluctuationRate){
			int flucFact = rand() % 3;
			int flSpd;

			switch(flucFact){
				case 0:
					flSpd = -fluctuationAmmount;
					break;
				case 1:
					flSpd = 0;
					break;
				case 2:
					flSpd = fluctuationAmmount;
					break;
			}

			refinedsRate += fluctuationAmmount;
			researchRate += fluctuationAmmount;
			lastUpdateTime = getTime();
		}
	}

	void Trader::trade(Player *tradingPlayer, int rt, int ammount, bool buying){
		int tradeResRate;

		switch((ResourceType)rt){
			case ResourceType::REFINEDS:
				tradeResRate = refinedsRate;
				break;
			case ResourceType::RESEARCH:
				tradeResRate = researchRate;
				break;
		}

		if(buying && ammount * tradeResRate <= tradingPlayer->getResource(ResourceType::WEALTH)){
			tradingPlayer->updateResource((ResourceType)rt, ammount, true);
			tradingPlayer->updateResource(ResourceType::WEALTH, -ammount * tradeResRate, true);
		}
		else if(!buying && tradingPlayer->getResource((ResourceType)rt) >= ammount){
			tradingPlayer->updateResource((ResourceType)rt, -ammount, true);
			tradingPlayer->updateResource(ResourceType::WEALTH, ammount * tradeResRate, true);
		}
	}
}
