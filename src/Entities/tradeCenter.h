#ifndef TRADE_CENTER_H
#define TRADE_CENTER_H

#include "structure.h"

namespace battleship{
	class TradeCenter : public Structure{
		public:
			TradeCenter(Player*, int, vb01::Vector3, vb01::Quaternion, int);
		private:
	};
}

#endif
