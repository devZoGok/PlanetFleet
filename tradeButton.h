#ifndef TRADE_BUTTON_H
#define TRADE_BUTTON_H

#include "unitButton.h"

namespace battleship{
	class TradeButton : public UnitButton{
		public:
			enum class Type{BUY_REFINEDS, SELL_REFINEDS, BUY_RESEARCH, SELL_RESEARCH};

			TradeButton(vb01::Vector3, vb01::Vector2, std::string, int, std::string, Type, int);
			void onClick();
			void update();
		private:
			Type type;
			int amount;
			std::wstring tooltipInitDesc = L"";

			void editResourceTradeData(int&, bool&);
	};
}

#endif
