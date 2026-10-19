#ifndef RESOURCE_DEPOSIT_H
#define RESOURCE_DEPOSIT_H

#include "gameObject.h"

namespace battleship{
	class Extractor;

	class ResourceDeposit : public GameObject{
		public:
			ResourceDeposit(Player*, int, vb01::Vector3, vb01::Quaternion, int);
			~ResourceDeposit();
			inline int getAmount(){return ammount;}
			inline int getInitAmount(){return initAmount;}
			inline void decreaseAmount(int amnt){ammount -= amnt;}
			inline Extractor* getExtractor(){return extractor;}
			inline void setExtractor(Extractor *e){this->extractor = e;}
		private:
			int ammount, initAmount;
			Extractor *extractor = nullptr;

			void reinit();
	};
}

#endif
