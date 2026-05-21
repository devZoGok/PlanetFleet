#ifndef BACK_BUTTON_H
#define BACK_BUTTON_H

#include <button.h>

namespace battleship{
	class BackButton : public vb01Gui::Button{
		public:
			BackButton(vb01::Vector3, vb01::Vector2, std::string, std::string);
			void onClick();
		private:
			std::string screen;
	};
}

#endif
