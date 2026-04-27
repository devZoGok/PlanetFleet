#ifndef TAB_BUTTON_H
#define TAB_BUTTON_H

#include <button.h>

namespace battleship{
	class TabButton : public vb01Gui::Button{
		public:
			TabButton(vb01::Vector3, vb01::Vector2, std::string, std::string);
			void onClick();
		private:
			std::string screenScript;
	};
}

#endif
