#ifndef UNIT_LISTBOX_H
#define UNIT_LISTBOX_H

#include <listbox.h>

namespace battleship{
	class GameObjectListbox : public vb01Gui::Listbox{
		public:
			GameObjectListbox(bool, vb01::Vector3, vb01::Vector2, std::vector<std::string>, std::vector<int>, int, std::string);
			void onClose();
		private:
			bool unitListbox;
			std::vector<int> gameObjIds;
	};
}

#endif
