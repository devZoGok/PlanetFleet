#ifndef ADD_RESOURCE_COMMAND_H
#define ADD_RESOURCE_COMMAND_H

#include "abstractCommand.h"

namespace battleship{
	class AddResourceCommand : public AbstractCommand{
		public:
			AddResourceCommand(std::string argsStr) : AbstractCommand(argsStr){}
			void execute();
		private:
			void validate();

			int playerId, resourceId, resourceAmmount;
	};
}

#endif
