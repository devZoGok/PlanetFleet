#ifndef MAIN_MENU_BUTTON_H
#define MAIN_MENU_BUTTON_H

#include "pfButtonBase.h"

namespace battleship{
	class MainMenuButton : public PfButtonBase{
		public:
			MainMenuButton(vb01::Vector3, vb01::Vector2, std::string);
			void onClick();
		private:
	};
}

#endif
