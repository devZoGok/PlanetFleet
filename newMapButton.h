#ifndef NEW_MAP_BUTTON_H
#define NEW_MAP_BUTTON_H

#include "pfButtonBase.h"

namespace vb01Gui{
	class Textbox;
}

namespace battleship{
	class NewMapButton : public PfButtonBase{
		public:
			class OkButton : public PfButtonBase{
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
