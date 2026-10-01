#ifndef LOAD_MAP_BUTTON_H
#define LOAD_MAP_BUTTON_H

#include "pfButtonBase.h"

namespace vb01Gui{
	class Listbox;
}

namespace battleship{
	class LoadMapButton : public PfButtonBase{
		public:
			class OkButton : public PfButtonBase{
				public:
					OkButton(vb01::Vector3, vb01::Vector2, vb01Gui::Listbox*);
					void onClick();
				private:
					vb01Gui::Listbox *listbox;
			};

			LoadMapButton(vb01::Vector3, vb01::Vector2);
			void onClick();
		private:
	};
}

#endif
