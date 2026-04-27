#ifndef OFFER_BUTTON_H
#define OFFER_BUTTON_H

#include <button.h>

namespace battleship{
	class OfferButton : public vb01Gui::Button{
		public:
			OfferButton(vb01::Vector3, vb01::Vector2, int, std::string, int, std::string);
			void onClick();
			inline void setPlayerId(int plId){playerId = plId;}
		private:
			int playerId;
	};
}

#endif
