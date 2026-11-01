#ifndef DEFAULTS_BUTTON_H
#define DEFAULTS_BUTTON_H

#include <vector>

#include "pfButtonBase.h"
#include "optionBinding.h"

namespace battleship{
	class DefaultsButton : public PfButtonBase{
		public:
			DefaultsButton(vb01::Vector3, vb01::Vector2, std::string, std::vector<OptionBinding>);
			void onClick();
		private:
			std::vector<OptionBinding> optionBindings;
	};
}

#endif
