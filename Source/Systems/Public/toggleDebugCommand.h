#ifndef TOGGLE_DEBUG_COMMAND_H
#define TOGGLE_DEBUG_COMMAND_H

#include "abstractCommand.h"

namespace battleship{
	class ToggleDebugCommand : public AbstractCommand{
		public:
			ToggleDebugCommand(std::string str) : AbstractCommand(str){}
			void validate();
			void execute();
		private:
	};
}

#endif
