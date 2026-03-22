#ifndef STATS_BUTTON_H
#define STATS_BUTTON_H

#include <button.h>

namespace battleship {
	class StatsButton : public vb01Gui::Button{
		public:
			StatsButton(vb01::Vector3, vb01::Vector2, std::string, int, std::string);
			void onClick();
		private:
			void addPlayerDataGuiElements(int);
	};
}

#endif
