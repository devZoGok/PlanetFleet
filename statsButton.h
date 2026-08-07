#ifndef STATS_BUTTON_H
#define STATS_BUTTON_H

#include "pfButtonBase.h"

namespace battleship {
	class StatsButton : public PfButtonBase{
		public:
			StatsButton(vb01::Vector3, vb01::Vector2, std::string, int, std::string);
			void onClick();
		private:
			void addPlayerDataGuiElements(int);
	};
}

#endif
