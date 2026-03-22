#ifndef MINIMAP_BUTTON_H
#define MINIMAP_BUTTON_H

#include "activeStateButton.h"

namespace vb01{
	class Node;
}

namespace battleship{
	class MinimapButton : public ActiveStateButton{
		public:
			MinimapButton(vb01::Vector3 pos, vb01::Vector2 size, std::string ip) : ActiveStateButton(pos, size, "", "minimap", "", -1, ip){}
			~MinimapButton(){}
			void onClick();
		private:
	};
}

#endif
