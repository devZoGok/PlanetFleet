#ifndef WEAPON_H
#define WEAPON_H

#include "unit.h"

#include <vector>
#include <util.h>
#include <quaternion.h>

#include <solUtil.h>

namespace vb01{
	class Node;
}

namespace battleship{
	class FxManager;
	class GameObject;

	class Weapon{
		public:
			enum class Type{DAMAGE = 0, FREEZER = 1};

			struct Component{
				vb01::Node *node = nullptr;
				vb01::Vector3 initUnitSpaceDir;
				float rotSpeed, minAngle, maxAngle;
				bool vertical;

				Component(vb01::Node *n, float rs, float min, float max, bool v) :
					node(n), 
					rotSpeed(rs), 
					minAngle(min), 
					maxAngle(max), 
					vertical(v) {}
			};

			Weapon(Unit*, sol::table, int);
			~Weapon();
			virtual void update();
			virtual void fire(Order);
			void trackTarget(vb01::Vector3);
			inline int getProjectileId(){return projId;}
			inline int getRateOfFire(){return rateOfFire;}
			inline int getDamage(){return damage;}
			inline int getMinRange(){return minRange;}
			inline int getMaxRange(){return maxRange;}
			inline Unit* getUnit(){return unit;}
			inline Order::TYPE getOrderType(){return orderType;}
		private:
			Unit *unit = nullptr;
			Order::TYPE orderType;
			int id, projId = -1, damage = 0;
			float minRange = 0, maxRange, maxFireAngle;
			vb01::s64 lastFireTime = 0;
			FxManager::Fx *fireFx = nullptr;
			vb01::Quaternion projRot;
			vb01::Vector3 projPos, initUnitSpaceDir; 
			vb01::Node *projPar = nullptr;
			static std::string LASER_FLAG;
			Type type = Type::DAMAGE;

			void initProjectileData(sol::table);
			void initNodes(sol::table);
			void useFx(FxManager::Fx*, vb01::Vector3, bool);
			inline bool canFire(){return vb01::getTime() - lastFireTime > rateOfFire;}
		protected:
			int rateOfFire;
			std::vector<Component> components;

			virtual void updateTarget(GameObject*);
	};
}

#endif
