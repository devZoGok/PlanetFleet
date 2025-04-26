#ifndef TRADE_OFFER_H
#define TRADE_OFFER_H

#include "player.h"

namespace battleship{
	struct TradeOffer{
		bool sellerAggrees = true, sellerDelivers = true, buyerAggrees = true, buyerDelivers = false;
		int tradeResources[NUM_RESOURCES][2], deliveredResources[NUM_RESOURCES][2];

		TradeOffer(int br, int sr, int bw, int sw, int bt, int st){
			tradeResources[(int)ResourceType::REFINEDS][0] = br;
			tradeResources[(int)ResourceType::REFINEDS][1] = sr;
			tradeResources[(int)ResourceType::WEALTH][0] = bw;
			tradeResources[(int)ResourceType::WEALTH][1] = sw;
			tradeResources[(int)ResourceType::RESEARCH][0] = bt;
			tradeResources[(int)ResourceType::RESEARCH][1] = st;

			for(int i = 0; i < NUM_RESOURCES; i++){
				deliveredResources[i][0] = 0;
				deliveredResources[i][1] = 0;
			}
		}
	};
}

#endif
