#ifndef TRADING_SCREEN_BUTTON_H
#define TRADING_SCREEN_BUTTON_H

#include "activeStateButton.h"

namespace vb01Gui{
	class Listbox;
}

namespace battleship{
	class TradingScreenButton : public ActiveStateButton{
		public:
			TradingScreenButton(vb01::Vector3, vb01::Vector2, vb01Gui::Listbox*, int, std::string, std::string, int, std::string);
			void onClick();
		private:
			int playerId;
			vb01Gui::Listbox *listbox;
	};
}

#endif
