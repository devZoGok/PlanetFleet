#ifndef ABSTRACT_COMMAND_H
#define ABSTRACT_COMMAND_H

#include <string>
#include <vector>

namespace battleship{
	class AbstractCommand{
		protected:
			AbstractCommand(std::string str) : cmdStr(str){}
			virtual void handle();
			virtual void validate(){}
			virtual void execute();

			std::string cmdStr;
			std::vector<std::string> arguments;
	};
}

#endif
