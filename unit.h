#ifndef UNIT_H
#define UNIT_H

#include <vector>
#include <util.h>
#include <quaternion.h>
#include <lineRenderer.h>

#include <solUtil.h>

#include "gameObject.h"
#include "buildableUnit.h"
#include "fxManager.h"

namespace sf{
	class SoundBuffer;
	class Sound;
}

namespace vb01{
	class Mesh;
	class Quad;
	class Node;
	class Light;
	class Camera;
}

namespace battleship{
    class Player;
	class Unit;
	class Vehicle;
	class Structure;
	class Factory;
	class Cruiser;
	class Engineer;
	class PointDefense;
	class Projectile;
    
    struct Order {
        enum class TYPE {ATTACK, BUILD, MOVE, GARRISON, EJECT, PATROL, LAUNCH, SUPPLY, LOAD, UNLOAD, HACK};
			struct Target{
				Unit *unit = nullptr;
				vb01::Vector3 pos;

				Target(){}
				Target(Unit *u, vb01::Vector3 p = vb01::Vector3::VEC_ZERO) : unit(u), pos(p){}
			};

        TYPE type;
		int lineId = -1;
		bool playerAssigned = true;
		vb01::Vector3 direction;
        std::vector<Target> targets;

		Order(){}
		Order(TYPE t, std::vector<Target> targ, vb01::Vector3 dir = vb01::Vector3::VEC_ZERO, int lid = -1, bool pa = true) :
		   	type(t), 
			playerAssigned(pa), 
			lineId(lid), 
			targets(targ), 
			direction(dir){}
    };

    enum class MoveDir {LEFT, UP, FORW};
    enum class Corner {FRONT_LEFT, FRONT_RIGHT, REAR_LEFT, REAR_RIGHT};
    enum class UnitType {UNDERWATER, SEA_LEVEL, HOVER, LAND, AIR, NONE = -1};
    enum class UnitClass {
		WAR_MECH,
	   	TANK,
	   	ARTILLERY,
	   	ENGINEER,
	   	TRANSPORT,
	   	RESOURCE_ROVER,
	   	CRUISER,
	   	CARRIER,
	   	SUBMARINE,
	   	LAND_FACTORY,
	   	NAVAL_FACTORY,
	   	TRADE_CENTER,
	   	LAB,
	   	POINT_DEFENSE,
	   	EXTRACTOR,
	   	REFINERY,
		FORT
	};
    
    class Unit : public GameObject{
    public:
		class Weapon{
			public:
				enum class Type{HITSCAN, SHELL, TORPEDO, CRUISE_MISSILE, HACK};

				Weapon(Unit*, sol::table, int);
				~Weapon();
				virtual void update();
				virtual void fire(Order);
				void trackTarget(vb01::Vector3);
				inline Type getType(){return type;}
				inline int getProjectileId(){return projId;}
				inline int getRateOfFire(){return rateOfFire;}
				inline int getDamage(){return damage;}
				inline int getMinRange(){return minRange;}
				inline int getMaxRange(){return maxRange;}
				inline Unit* getUnit(){return unit;}
				inline Order::TYPE getOrderType(){return orderType;}
			private:
				Type type;
				int id, projId = -1, rateOfFire, damage = 0;
				float minRange = 0, maxRange, rotSpeed, maxFireAngle;
				vb01::Vector3 projPos;
				vb01::Quaternion projRot, rot = vb01::Quaternion::QUAT_W;
				vb01::s64 lastFireTime = 0;
				Unit *unit = nullptr;
				FxManager::Fx *fireFx = nullptr;
				vb01::Node *node = nullptr;
				Order::TYPE orderType;
				vb01::Vector3 upVec = vb01::Vector3(0, 1, 0), dirVec = vb01::Vector3(0, 0, 1), leftVec = vb01::Vector3(1, 0, 0);
				static std::string LASER_FLAG;

				void initProjectileData(sol::table);
				void initNode(sol::table);
				FxManager::Fx* initFx(sol::table, std::string, bool);
				void useFx(FxManager::Fx*, vb01::Vector3, bool);
				inline bool canFire(){return vb01::getTime() - lastFireTime > rateOfFire;}
		};

		struct GarrisonSlot{
			Vehicle *vehicle = nullptr;
			int category;
			vb01::Node *background, *foreground;
			vb01::Vector2 offset;

			GarrisonSlot(vb01::Node *bg, vb01::Node *fg, vb01::Vector2 off, int cat, Vehicle *v = nullptr) : background(bg), foreground(fg), offset(off), category(cat), vehicle(v){}
		};

		enum class Armor {CAST, COMBINED, MECHANIC, SHELL, STEEL};
		enum class State {CHASE, STAND_GROUND, HOLD_FIRE};

        Unit(Player*, int, vb01::Vector3, vb01::Quaternion, State = State::STAND_GROUND);
        virtual ~Unit();
        virtual void update();
        virtual void halt();
		void updateGarrison(Vehicle*, bool);
        virtual void select();
        void setOrder(Order);
        std::vector<Projectile*> getProjectiles();
        virtual void receiveOrder(Order, bool);
		bool canGarrison(Vehicle*);
		void initLosLight();
		void destroyLosLight();
		inline void setState(State s){state = s;}
		inline State getState(){return state;}
		inline Engineer* toEngineer(){return (Engineer*)this;}
		inline Structure* toStructure(){return (Structure*)this;}
		inline Factory* toFactory(){return (Factory*)this;}
		inline Cruiser* toCruiser(){return (Cruiser*)this;}
		inline PointDefense* toPointDefense(){return (PointDefense*)this;}
		inline int getNumGarrisonSlots(){return garrisonSlots.size();}
		inline const std::vector<GarrisonSlot>& getGarrisonSlots(){return garrisonSlots;}
        inline float getLineOfSight() {return lineOfSight;}
        inline UnitType getType() {return type;}
        inline UnitClass getUnitClass() {return unitClass;}
        inline void takeDamage(int damage) {health -= damage;}
        inline int getPlayerId() {return playerId;}
		inline int getHealth(){return health;}
		inline int getDeathHp(){return DEATH_HP;}
		inline bool isVehicle(){return vehicle;}
		inline bool isTargetToTheRight(vb01::Vector3 dir, vb01::Vector3 lv){return lv.getAngleBetween(dir) > vb01::PI / 2;}
		inline Order getOrder(int i){return orders[i];}
		inline int getNumOrders(){return orders.size();}
		inline std::string getGuiScreen(){return guiScreen;}
		inline BuildableUnit getBuildableUnit(int i){return buildableUnits[i];}
		inline vb01::Node* getLosLightNode(){return losLightNode;}
    private:
		void renderOrderLine(bool);
        void updateScreenCoordinates();
		void initWeapons();
		void destroyWeapons();
		bool validateOrder(Order);
        inline bool canDisplayOrderLine(){return vb01::getTime() - orderLineDispTime < orderVecDispLength;}

        const int orderVecDispLength = 2000, DEATH_HP = 0;
        sf::SoundBuffer *selectionSfxBuffer;
        sf::Sound *selectionSfx = nullptr;
		vb01::Node *hpBackgroundNode = nullptr, *hpForegroundNode = nullptr, *losLightNode = nullptr;
		bool vehicle;
    protected:
        UnitClass unitClass;
        UnitType type;
        std::vector<Order> orders;
		std::string guiScreen = "";
        int health = 0, maxHealth, playerId, lenHpBar = 200;
		vb01::s64 orderLineDispTime = 0, lastFireTime = 0;
        float lineOfSight;
		std::vector<Armor> armorTypes;
		std::vector<Weapon*> weapons;
		std::vector<GarrisonSlot> garrisonSlots;
		std::vector<BuildableUnit> buildableUnits;
		State state = State::STAND_GROUND;

		std::vector<Player*> getSelectingPlayers();
        void removeOrder(int);
		virtual bool validateGarrisonOrder(Order){return false;}
		virtual void targetUnitsAutomatically();
		virtual void reinit();
		virtual void initProperties();
		virtual void destroySound();
		virtual void initSound();
		virtual void initUnitStats();
        virtual void executeOrders();
        virtual void eject(Order);
        virtual void attack(Order);
        virtual void garrison(Order){}
        virtual void build(Order){}
        virtual void move(Order){}
        virtual void patrol(Order){}
        virtual void launch(Order);
		virtual void handleResources(Order){}
		virtual void hack(Order){}
		float calculateRotation(vb01::Vector3, float, float);
		void removeBar(vb01::Node*);
		vb01::Node* createBar(vb01::Vector2, vb01::Vector2, vb01::Vector4);
        void displayUnitStats(vb01::Node*, vb01::Node*, int, int, bool, vb01::Vector2 offset = vb01::Vector2::VEC_ZERO);
		std::vector<Weapon*> getWeaponsByOrder(Order::TYPE);
		inline std::vector<Armor> getArmorTypes(){return armorTypes;}
    };
}

#endif
