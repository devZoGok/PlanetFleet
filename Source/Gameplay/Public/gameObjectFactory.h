#ifndef GAME_OBJECT_FACTORY_H
#define GAME_OBJECT_FACTORY_H

#include <vector.h>
#include <quaternion.h>

namespace battleship{
	class Unit;
	class Projectile;
	class ResourceDeposit;
	class Player;

	class GameObjectFactory{
		public:
			enum GameObjectType{UNIT, PROJECTILE, RESOURCE_DEPOSIT};
			static Unit* createUnit(Player*, int, vb01::Vector3, vb01::Quaternion, int = 0);
			static Projectile* createProjectile(Unit*, int, vb01::Vector3, vb01::Quaternion);
			static ResourceDeposit* createResourceDeposit(Player*, int, vb01::Vector3, vb01::Quaternion, int = 0);
		private:
	};
}

#endif
