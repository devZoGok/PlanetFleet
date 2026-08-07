#ifndef SINGLE_PLAYER_BUTTON_H
#define SINGLE_PLAYER_BUTTON_H

#include <vector>

#include <button.h>

namespace battleship{
	class Tooltip;

    class SinglePlayerButton : public vb01Gui::Button {
    public:
		SinglePlayerButton(vb01::Vector3, vb01::Vector2, std::string);
		~SinglePlayerButton();
		void onClick();
		void update();
		inline void setTooltip(Tooltip *t){this->tooltip = t;}
    private:
		Tooltip *tooltip = nullptr;
    };
}

#endif
