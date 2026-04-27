#include "console.h"
#include "addUnitCommand.h"
#include "addResourceCommand.h"
#include "addTechnologyCommand.h"
#include "toggleDebugCommand.h"

using namespace std;

namespace battleship{
    void Console::execute(string cmdStr) {
		int space = cmdStr.find(" ");
		string cmdName = cmdStr, argsStr = "";

		if(space != -1){
			cmdName = cmdStr.substr(0, space);
			argsStr = cmdStr.substr(space + 1, string::npos);
		}

		if(cmdName == "add-unit")
			AddUnitCommand(argsStr).execute();
		else if(cmdName == "add-resource")
			AddResourceCommand(argsStr).execute();
		else if(cmdName == "add-technology")
			AddTechnologyCommand(argsStr).execute();
		else if(cmdName == "toggle-debug")
			ToggleDebugCommand(argsStr).execute();
    }
}
