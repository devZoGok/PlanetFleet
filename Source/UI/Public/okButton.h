#ifndef OK_BUTTON_H
#define OK_BUTTON_H

#include <button.h>

namespace battleship{
	class OkButton : public vb01Gui::Button{
		public:
			OkButton(vb01::Vector3, vb01::Vector2, std::string);
			void onClick();
		private:
	};
}

#endif
