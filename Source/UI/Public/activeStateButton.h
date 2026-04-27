#ifndef ACTIVE_STATE_BUTTON_H
#define ACTIVE_STATE_BUTTON_H

#include <button.h>

namespace battleship{
	class ActiveStateButton : public vb01Gui::Button{
		public:
			ActiveStateButton(vb01::Vector3, vb01::Vector2, std::string, std::string, std::string, int = -1, std::string = "");
			void onClick();
		private:
			std::string guiScreen = "";
		protected:
			std::vector<vb01Gui::Button*> buttons;
	};
}

#endif
