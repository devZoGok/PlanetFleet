#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <algorithm>

#include "gameManager.h"
#include "trader.h"
#include "unit.h"

namespace battleship{
	class ResourceDeposit;
	class Projectile;
	class Unit;
	struct TradeOffer;

	const int NUM_RESOURCES = 3;
	enum class ResourceType{REFINEDS, WEALTH, RESEARCH};

    class Player {
    public:
        Player(int, int, int, vb01::Vector3, bool = true, int = -1, std::string = "");
        ~Player();
        void update();
		void issueOrder(Order::TYPE, vb01::Vector3, std::vector<Order::Target>, bool);
		void removeUnit(Unit*);
		void removeUnit(int);
		void removeResourceDeposit(int);
		void removeProjectile(int);
		void removeProjectile(Projectile*);
        bool isThisPlayersUnit(GameObject*);
		void selectUnits(std::vector<Unit*>);
		std::vector<Unit*> getUnitsById(int, int = -1);
		std::vector<Unit*> getUnitsByClass(UnitClass, int = -1);
		void addTechnology(int id){technologies.push_back(id);}
		void updateTradedResource(Player*, ResourceType, int, bool, bool);
		void addTradeOffer(Player*, TradeOffer*);
		std::vector<TradeOffer*> getTradeOffers(Player*);
		inline int getResource(ResourceType rt){return resources[(int)rt];}
		inline void updateResource(ResourceType rt, int amount, bool add){resources[(int)rt] = (add ? resources[(int)rt] + amount : amount);}
		inline Trader* getTrader(){return trader;}
		inline void deselectUnits(){selectedUnits.clear();}
		inline Unit* getSelectedUnit(int id){return selectedUnits[id];}
		inline std::vector<Unit*> getSelectedUnits(){return selectedUnits;}
		inline void deselectUnit(int i){selectedUnits.erase(selectedUnits.begin() + i);}
		inline int getNumSelectedUnits(){return getSelectedUnits().size();}
		inline void selectUnit(Unit *u){selectUnits(std::vector<Unit*>{u});}
        inline void addUnit(Unit *u){units.push_back(u);}
		inline std::vector<ResourceDeposit*>& getResourceDeposits(){return resourceDeposits;}
        inline void addResourceDeposit(ResourceDeposit *rd){resourceDeposits.push_back(rd);}
		inline int getNumResourceDeposits(){return resourceDeposits.size();}
		inline void addProjectile(Projectile *proj){projectiles.push_back(proj);}
		inline int getNumProjectiles(){return projectiles.size();}
        inline std::vector<Projectile*>& getProjectiles(){return projectiles;}
        inline Unit* getUnit(int i){return units[i];}
        inline std::vector<Unit*>& getUnits(){return units;}
        inline void setTeam(int t){team = t;}
        inline int getTeam(){return team;}
        inline int getNumUnits(){return units.size();}
        inline int getFaction(){return faction;}
        inline int getSpawnPointId(){return spawnPointId;}
		inline bool isCpuPlayer(){return cpuPlayer;}
		inline int getNumVehiclesBuilt(){return vehiclesBuilt;}
		inline int getNumVehiclesDestroyed(){return vehiclesDestroyed;}
		inline int getNumVehiclesLost(){return vehiclesLost;}
		inline int getNumStructuresBuilt(){return structuresBuilt;}
		inline int getNumStructuresDestroyed(){return structuresDestroyed;}
		inline int getNumStructuresLost(){return structuresLost;}
		inline void incVehiclesBuilt(){vehiclesBuilt++;}
		inline void incVehiclesDestroyed(){vehiclesDestroyed++;}
		inline void incVehiclesLost(){vehiclesLost++;}
		inline void incStructuresBuilt(){structuresBuilt++;}
		inline void incStructuresDestroyed(){structuresDestroyed++;}
		inline void incStructuresLost(){structuresLost++;}
		inline vb01::Vector3 getColor(){return color;}
		inline std::string getName(){return name;}
		inline vb01::Material* getColorMaterial(){return colorMaterial;}
		inline std::vector<int> getTechnologies(){return technologies;}
    private:
		struct TradedResource{
			ResourceType type;
			int taken = 0, given = 0, received = 0, hadTaken = 0;

			TradedResource(ResourceType t) : type(t){}
		};

		bool cpuPlayer = false;
		std::vector<int> technologies;
		int luaPlayerId;
        int resources[3]{0, 0, 0};
		int faction, difficulty, team;
		int vehiclesBuilt = 0, vehiclesDestroyed = 0, vehiclesLost = 0;
		int structuresBuilt = 0, structuresDestroyed = 0, structuresLost = 0;
        int spawnPointId = -1;
		Trader *trader = nullptr; 
		std::string name;
        std::vector<Unit*> units, selectedUnits;
		std::vector<Projectile*> projectiles;
		std::vector<ResourceDeposit*> resourceDeposits;
        vb01::Vector3 color;
		vb01::Material *colorMaterial = nullptr;
		std::vector<std::pair<Player*, std::vector<TradeOffer*>>> tradeOffers;
		std::vector<std::pair<Player*, std::vector<TradedResource>>> tradedResources;

		int getOrderLineId(Order::TYPE, vb01::Vector3, vb01::Vector3);
		void initTradingVecs();
    };
}

#endif
