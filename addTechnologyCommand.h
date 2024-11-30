#ifndef ADD_TECHNOLOGY_H
#define ADD_TECHNOLOGY_H

#include "abstractCommand.h"

namespace battleship{
	class AddTechnologyCommand : public AbstractCommand{
		public:
			AddTechnologyCommand(std::string argsStr) : AbstractCommand(argsStr){}
			void execute();
		private:
			void validate();

			int playerId, techId;
	};
}

#endif
