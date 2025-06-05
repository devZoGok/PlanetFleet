#ifndef WEAPON_H
#define WEAPON_H

#include <vector>
#include <util.h>
#include <quaternion.h>

#include <solUtil.h>

#include "unit.h"

namespace vb01{
	class Node;
}

namespace battleship{
	class FxManager;

	class Weapon{
		public:
			enum class Type{FREEZER = 1};

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
			float minRange = 0, maxRange, rotSpeed, maxFireAngle;
			vb01::s64 lastFireTime = 0;
			FxManager::Fx *fireFx = nullptr;
			vb01::Quaternion projRot;
			vb01::Vector3 projPos;
			float horConstraint = 1.57, vertConstraint = 1.57;
			vb01::Node *horNode = nullptr, *vertNode = nullptr, *projPar = nullptr;
			static std::string LASER_FLAG;

			vb01::Vector3 calcOrientVec(int);
			void alignNode(vb01::Vector3, vb01::Node*, bool);
			void initProjectileData(sol::table);
			vb01::Node* initNode(sol::table, bool);
			FxManager::Fx* initFx(sol::table, std::string, bool);
			void useFx(FxManager::Fx*, vb01::Vector3, bool);
			inline bool canFire(){return vb01::getTime() - lastFireTime > rateOfFire;}
		protected:
			int rateOfFire;

			void updateTargetUnit(Unit*);
	};

	class Freezer : public Weapon{
		public:
			Freezer(Unit*, sol::table, int);
		private:
			vb01::s64 lastFreezeTime = 0;

			void updateTargetUnit(Unit*);
			inline bool canFreeze(){return vb01::getTime() - lastFreezeTime > rateOfFire;}
	};
}

#endif
