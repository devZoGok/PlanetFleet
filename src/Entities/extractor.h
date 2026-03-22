#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include "structure.h"

#include <util.h>

namespace battleship{
	class ResourceDeposit;

	class Extractor : public Structure{
		public:
			Extractor(Player*, int, vb01::Vector3, vb01::Quaternion, int, ResourceDeposit *rd = nullptr, Unit::State = Unit::State::STAND_GROUND);
			~Extractor();
			void update();
			void draw();
			bool canDraw(){return vb01::getTime() - lastDrawTime > drawRate;}
			inline ResourceDeposit* getDeposit(){return deposit;}
		private:
			void initProperties();

			int drawSpeed, drawRate;
			vb01::s64 lastDrawTime = 0;
			vb01::Node *ammountBackground = nullptr, *ammountForeground = nullptr;
			ResourceDeposit *deposit = nullptr;

	};
}

#endif
