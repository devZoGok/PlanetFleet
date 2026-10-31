#ifndef UNIT_BUTTON_H
#define UNIT_BUTTON_H

#include "pfButtonBase.h"

namespace battleship{
	class Unit;

	class UnitButton : public PfButtonBase{
		public:
			UnitButton(vb01::Vector3, vb01::Vector2, std::string, std::string, int, std::string);
		protected:
			std::vector<Unit*> getUnits();
	};
}

#endif
