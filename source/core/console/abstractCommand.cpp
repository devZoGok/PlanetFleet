#include "abstractCommand.h"

#include <cmath>
#include <vector.h>

namespace battleship{
	using namespace std;

	void AbstractCommand::handle(){
		if(cmdStr.find(" ") == -1)
			return;

		vector<int> spaceIds;
		arguments.clear();

		for(int i = 0; i < cmdStr.length(); i++)
			if(cmdStr[i] == ' ')
				spaceIds.push_back(i);

		arguments.push_back(cmdStr.substr(0, spaceIds[0]));

		for(int i = 0; i < spaceIds.size(); i++){
			bool lastSpace = (i == spaceIds.size() - 1);
			string argument = cmdStr.substr(spaceIds[i] + 1, lastSpace ? string::npos : spaceIds[i + 1] - spaceIds[i] - 1);
			arguments.push_back(argument);
		}
	}

	void AbstractCommand::execute(){
		handle();
		validate();
	}
}
