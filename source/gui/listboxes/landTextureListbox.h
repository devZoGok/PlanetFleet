#ifndef LAND_TEXTURE_LISTBOX_H
#define LAND_TEXTURE_LISTBOX_H

#include <listbox.h>

namespace battleship{
	class LandTextureListbox : public vb01Gui::Listbox{
		public:
			LandTextureListbox(vb01::Vector3, vb01::Vector2, std::vector<std::string>, int, std::string);
			void onClose();
		private:
	};
}

#endif

