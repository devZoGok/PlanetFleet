#ifndef RESOURCE_AMMOUNT_BUTTON_H
#define RESOURCE_AMMOUNT_BUTTON_H

#include <button.h>

namespace vb01Gui{
	class Textbox;
}

namespace battleship{
	class ResourceAmmountButton : public vb01Gui::Button{
		public:
			ResourceAmmountButton(vb01::Vector3, vb01::Vector2, std::string, int, int, std::string);
			void onClick();
		private:
			vb01Gui::Textbox *textbox = nullptr;
			int ammount, minAmmount = 0, maxAmmount = 10000;
	};
}

#endif
