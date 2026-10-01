#ifndef MAP_EDITOR_BUTTON_H
#define MAP_EDITOR_BUTTON_H

#include "pfButtonBase.h"
#include "gameManager.h"

namespace battleship{
	class MapEditorButton : public PfButtonBase{
		public:
			MapEditorButton(vb01::Vector3 pos, vb01::Vector2 size) : PfButtonBase(pos, size, "Map editor", GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", -1, true){}
			void onClick();
		private:
	};	
}

#endif
