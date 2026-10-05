#ifndef OPTION_BINDING_H
#define OPTION_BINDING_H

#include <solUtil.h>

#include <string>

#include "concreteGuiManager.h"

namespace battleship{
	//ties a GUI element to an entry of Scripts/Core/options.lua
	class OptionBinding{
		public:
			OptionBinding(GuiElementType, void*, sol::table);
			void load();
			void store();
			void restoreDefault();
			static void save();
			static void apply();
		private:
			GuiElementType type;
			void *guiElement;
			sol::table option;
			std::string path;
	};
}

#endif
