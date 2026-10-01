#ifndef OK_BUTTON_H
#define OK_BUTTON_H

#include <vector>

#include "pfButtonBase.h"
#include "optionBinding.h"

namespace battleship{
	class OkButton : public PfButtonBase{
		public:
			OkButton(vb01::Vector3, vb01::Vector2, std::string, std::vector<OptionBinding>);
			void onClick();
		private:
			std::vector<OptionBinding> optionBindings;
	};
}

#endif
