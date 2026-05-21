#ifndef ICE_SHEET_H
#define ICE_SHEET_H

#include "structure.h"

namespace battleship{
	class IceSheet : public Structure{
		public:
			IceSheet(Player*, int, vb01::Vector3, vb01::Quaternion, int);
			void update();
		private:
	};
}

#endif
