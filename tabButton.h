#ifndef TAB_BUTTON_H
#define TAB_BUTTON_H

#include "pfButtonBase.h"

namespace battleship{
	class TabButton : public PfButtonBase{
		public:
			TabButton(vb01::Vector3, vb01::Vector2, std::string, std::string);
			void onClick();
		private:
			std::string screenScript;
	};
}

#endif
