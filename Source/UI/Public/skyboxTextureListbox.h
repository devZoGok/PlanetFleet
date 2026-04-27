#ifndef SKYBOX_TEXTURE_LISTBOX_H
#define SKYBOX_TEXTURE_LISTBOX_H

#include <listbox.h>

namespace battleship{
	class SkyboxTextureListbox : public vb01Gui::Listbox{
		public:
			SkyboxTextureListbox(vb01::Vector3, vb01::Vector2, std::vector<std::string>, int, std::string);
			void onClose();
		private:
	};
}

#endif

