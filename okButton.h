#ifndef OK_BUTTON_H
#define OK_BUTTON_H

#include "pfButtonBase.h"

namespace battleship{
	class OkButton : public PfButtonBase{
		public:
			OkButton(vb01::Vector3, vb01::Vector2, std::string);
			void onClick();
		private:
	};
}

#endif
