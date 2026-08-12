#ifndef STATE_TOGGLE_BUTTON_H
#define STATE_TOGGLE_BUTTON_H

#include "pfButtonBase.h"

namespace battleship{
	class StateToggleButton : public PfButtonBase{
		public:
			StateToggleButton(vb01::Vector3, vb01::Vector2, std::string, int, std::string);
			void onClick();
		private:
			void updateStateId();
			void toggleImage();
			
			int currStateId, x;
	};
}

#endif
