#ifndef MAP_EDITOR_BUTTON_H
#define MAP_EDITOR_BUTTON_H

#include <button.h>

#include "gameManager.h"

namespace battleship{
	class MapEditorButton : public vb01Gui::Button{
		public:
			MapEditorButton(vb01::Vector3 pos, vb01::Vector2 size) : vb01Gui::Button(pos, size, "Map editor", GameManager::getSingleton()->getPath() + "Fonts/batang.ttf"){}
			void onClick();
		private:
	};	
}

#endif
