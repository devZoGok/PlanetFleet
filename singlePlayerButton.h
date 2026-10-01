#ifndef SINGLE_PLAYER_BUTTON_H
#define SINGLE_PLAYER_BUTTON_H

#include <vector>

#include "pfButtonBase.h"

namespace battleship{
	class Tooltip;

    class SinglePlayerButton : public PfButtonBase {
    public:
		SinglePlayerButton(vb01::Vector3, vb01::Vector2, std::string);
		~SinglePlayerButton(){}
		void onClick();
    };
}

#endif
