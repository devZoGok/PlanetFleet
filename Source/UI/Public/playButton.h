#ifndef PLAY_BUTTON_H
#define PLAY_BUTTON_H

#include <button.h>

namespace vb01Gui{
	class Listbox;
}

namespace battleship{
	class PlayButton : public vb01Gui::Button {
	public:
	    PlayButton(vb01Gui::Listbox*, vb01::Vector3, vb01::Vector2, std::string, bool);
	    void onClick();
	private:
		vb01Gui::Listbox *mapListbox = nullptr;
	};
}

#endif
