#ifndef DEFAULTS_BUTTON_H
#define DEFAULTS_BUTTON_H

#include <button.h>

namespace battleship{
	class DefaultsButton : public vb01Gui::Button{
		public:
			DefaultsButton(vb01::Vector3, vb01::Vector2, std::string);
			void onClick();
		private:
	};
}

#endif
