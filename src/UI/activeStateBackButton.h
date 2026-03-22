#ifndef ACTIVE_STATE_BACK_BUTTON_H
#define ACTIVE_STATE_BACK_BUTTON_H

#include "backButton.h"

namespace battleship{
	class ActiveStateBackButton : public BackButton{
		public:
			ActiveStateBackButton(vb01::Vector3, vb01::Vector2, std::string);
			void onClick();
		private:
	};
}

#endif
