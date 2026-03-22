#ifndef MAP_LISTBOX_H
#define MAP_LISTBOX_H

#include <listbox.h>

namespace vb01Gui{
	class Listbox;
}

namespace battleship{
	class MapListbox : public vb01Gui::Listbox{
		public:
			MapListbox(vb01::Vector3, vb01::Vector2, std::vector<std::string>&, int, bool, std::string, bool);
		private:
			void onClose();

			bool addPlayerGui;
			std::vector<vb01Gui::Listbox*> cpuPlayerListboxes;
	};
}

#endif
