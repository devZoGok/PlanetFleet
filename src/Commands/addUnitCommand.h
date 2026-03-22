#ifndef ADD_UNIT_COMMAND_H
#define ADD_UNIT_COMMAND_H

#include "abstractCommand.h"

#include <vector>
#include <quaternion.h>

namespace battleship
{
	class AddUnitCommand : public AbstractCommand
	{
	public:
		AddUnitCommand(std::string argsStr) : AbstractCommand(argsStr) {}
		void validate();
		void execute();

	private:
		int playerId, unitId;
		bool posEnabled, rotEnabled;
		vb01::Vector3 pos = vb01::Vector3::VEC_ZERO;
		vb01::Quaternion rot = vb01::Quaternion::QUAT_W;
	};
}

#endif
