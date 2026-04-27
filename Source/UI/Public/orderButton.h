#ifndef ORDER_BUTTON_H
#define ORDER_BUTTON_H

#include <button.h>

namespace battleship{
	class OrderButton : public vb01Gui::Button{
		public:
			OrderButton(vb01::Vector3, vb01::Vector2, std::string, int, std::string, int);
			void onClick();
		private:
			int orderId;
	};
}

#endif
