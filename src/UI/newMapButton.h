#ifndef NEW_MAP_BUTTON_H
#define NEW_MAP_BUTTON_H

#include <button.h>

namespace vb01Gui{
	class Textbox;
}

namespace battleship{
	class NewMapButton : public vb01Gui::Button{
		public:
			class OkButton : public vb01Gui::Button{
				public:
					OkButton(vb01::Vector3, vb01::Vector2, vb01Gui::Textbox*, vb01Gui::Textbox*, vb01Gui::Textbox*);
					void onClick();
				private:
					vb01Gui::Textbox *name, *sizeX, *sizeY;
			};

			NewMapButton(vb01::Vector3, vb01::Vector2);
			void onClick();
		private:
	};
}

#endif
