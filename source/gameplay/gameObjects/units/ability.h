#ifndef ABILITY_H
#define ABILITY_H

#include <vector>

namespace battleship{
	struct Ability{
		enum class Type{
			SPEED,
		   	HEALTH,
		   	LINE_OF_SIGHT,
		   	MAX_TURN_ANGLE,
		   	DIRECT_HIT_DAMAGE,
		   	EXPLOSION_RADIUS,
		   	EXPLOSION_DAMAGE,
			CAPACITY,
			LOAD_RATE,
			LOAD_SPEED,
			DRAW_RATE,
			DRAW_SPEED,
			HACK_RANGE,
			UNIT_UNLOCK
		};

		Type type;
	   	float ammount;
		int gameObjType;
		std::vector<int> gameObjIds;
	};
}

#endif
