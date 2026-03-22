#ifndef SINGLE_PLAYER_BUTTON_H
#define SINGLE_PLAYER_BUTTON_H

#include <vector>

#include <button.h>

namespace battleship{
    class SinglePlayerButton : public vb01Gui::Button {
    public:
		SinglePlayerButton(vb01::Vector3, vb01::Vector2, std::string);
		void onClick();
		void onMouseOver();
		void onMouseOff();
    private:
    };
}

#endif
