#ifndef TRADER_H
#define TRADER_H

#include <util.h>

namespace battleship{
	class Player;

	class Trader{
		public:
			Trader();
			void update();
			void trade(Player*, int, int, bool);
		private:
			void initProperties();

			vb01::s64 lastUpdateTime = 0;
			int fluctuationRate, refinedsRate, researchRate, fluctuationAmmount;
	};
}

#endif
