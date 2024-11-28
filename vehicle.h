#ifndef VEHICLE_H
#define VEHICLE_H

#include "unit.h"

namespace vb01{
	class Material;
}

namespace battleship{
	class Vehicle : public Unit{
		public:
			Vehicle(Player*, int, vb01::Vector3, vb01::Quaternion, Unit::State);
			~Vehicle();
			virtual void update();
        	void move(Order);
			void exitGarrisonable(vb01::Vector3);
			inline Unit* getGarrisonable(){return garrisonable;}
			inline int getGarrisonCategory(){return garrisonCategory;}
		private:
		   	Unit *garrisonable = nullptr;
			int patrolPointId = 0, garrisonCategory;
        	float speed, maxTurnAngle, anglePrecision;
			vb01::Material *debugMat = nullptr;
			std::vector<vb01::Node*> debugPathPoints;

        	inline int getNextPatrolPointId(int numPoints) {return patrolPointId == numPoints - 1 ? 0 : patrolPointId + 1;}
			void enterGarrisonable();
			void halt();
			void turn(float);
			void receiveOrder(Order, bool);
			void advance(float, MoveDir = MoveDir::FORW);
			void addPathpoint(vb01::Vector3);
			void removePathpoint(int = 0);
			void removeAllPathpoints();
			void select();
			void reinit();
		protected:
			std::vector<vb01::Vector3> pathPoints;
			bool pursuingTarget = false;

        	void navigate(float = 0.);
			void navigateToTarget(float);
			void preparePathpoints(Order&, vb01::Vector3, bool = false);
			void alignToSurface();
			void attack(Order);
			void garrison(Order);
			void patrol(Order);
			virtual void initProperties();
	};
}

#endif
