#ifndef TRADE_OFFER_H
#define TRADE_OFFER_H

#include "player.h"

namespace battleship{
	struct TradeOffer{
		bool sellerAggrees = true, sellerDelivers = true, buyerAggrees = false, buyerDelivers = false;
		int sellRefineds = 0, buyRefineds = 0, sellWealth = 0, buyWealth = 0, sellResearch = 0, buyResearch = 0;

		TradeOffer(bool bd, bool sd, int br, int sr, int bw, int sw, int bt, int st) : 
			buyerDelivers(bd),
			sellerDelivers(sd),
			buyRefineds(br), 
			buyWealth(bw), 
			buyResearch(bt),
			sellRefineds(sr), 
			sellWealth(sw), 
			sellResearch(st) 
		{}
	};
}

#endif
