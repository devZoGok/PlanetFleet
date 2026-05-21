#ifndef PLAYER_TRADE_BUTTON_H
#define PLAYER_TRADE_BUTTON_H

#include "activeStateButton.h"

namespace battleship{
	class PlayerTradeButton : public ActiveStateButton{
		public:
			PlayerTradeButton(vb01::Vector3, vb01::Vector2, std::string, std::string, int, std::string);
			void onClick();
		private:
	};
}

#endif
