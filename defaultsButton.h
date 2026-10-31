#ifndef DEFAULTS_BUTTON_H
#define DEFAULTS_BUTTON_H

#include "pfButtonBase.h"

namespace battleship{
	class DefaultsButton : public PfButtonBase{
		public:
			DefaultsButton(vb01::Vector3, vb01::Vector2, std::string);
			void onClick();
		private:
	};
}

#endif
