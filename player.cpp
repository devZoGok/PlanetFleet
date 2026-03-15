#include <solUtil.h>
#include <stateManager.h>

#include "player.h"
#include "game.h"
#include "structure.h"
#include "vehicle.h"
#include "projectile.h"
#include "tradeOffer.h"
#include "destructable.h"
#include "activeGameState.h"
#include "resourceDeposit.h"

namespace battleship{
	using namespace vb01;
	using namespace gameBase;
	using namespace std;

    Player::Player(int diff, int fac, int t, Vector3 col, bool cpuPl, int sp, string n) : 
		difficulty(diff), 
		faction(fac), 
		team(t), 
		color(col), 
		cpuPlayer(cpuPl), 
		spawnPointId(sp), 
		name("pl"), 
		trader(new Trader())
	{
		resources[0] = 30000;

		colorMaterial = new Material(Root::getSingleton()->getLibPath() + "texture");
		colorMaterial->addBoolUniform("lightingEnabled", false);
		colorMaterial->addBoolUniform("texturingEnabled", false);
		colorMaterial->addVec4Uniform("diffuseColor", Vector4(color.x, color.y, color.z, 1));
	}

    Player::~Player() {
		delete colorMaterial;
	}

    void Player::update() {
		trader->update();

		vector<Unit*> units = this->units; 
		for(Unit *u : units){
			if(!u->isRemove()) u->update();
			else removeUnit(u);
		}

		vector<Projectile*> projectiles = this->projectiles; 
		for(Projectile *proj : projectiles){
			if(!proj->isRemove()) proj->update();
			else removeProjectile(proj);
		}

		for(ResourceDeposit *rd : resourceDeposits) rd->update();
		
		for(pair<Player*, vector<TradeOffer*>> &pair : tradeOffers){
			if(pair.second.empty()) continue;

			bool fulfiled = true;

			for(int i = 0; i < NUM_RESOURCES && fulfiled; i++){
				bool b = (pair.second[0]->tradeResources[i][0] == pair.second[0]->deliveredResources[i][0]);
				bool s = (pair.second[0]->tradeResources[i][1] == pair.second[0]->deliveredResources[i][1]);

				if(!(b && s)) fulfiled = false;
			}

			if(fulfiled)
				pair.second.erase(pair.second.begin());
		}
    }

	void Player::haltUnits(){
		for (Unit *u : selectedUnits)
			u->halt();
	}

	int Player::getOrderLineId(Order::TYPE type, Vector3 startPos, Vector3 endPos){
		Vector3 color;

      	switch(type){
      	    case Order::TYPE::MOVE:
				color = Vector3::VEC_J;
      	        break;
      	    case Order::TYPE::ATTACK:
				color = Vector3::VEC_I;
      	        break;
      	    case Order::TYPE::PATROL:
      	    case Order::TYPE::GARRISON:
      	    case Order::TYPE::EJECT:
				color = Vector3::VEC_K;
				break;
      	    case Order::TYPE::BUILD:
      	    case Order::TYPE::SUPPLY:
      	    case Order::TYPE::LOAD:
      	    case Order::TYPE::UNLOAD:
				color = Vector3(1, 1, 0);
      	        break;
      	    case Order::TYPE::HACK:
				color = Vector3(1, 0, 1);
      	        break;
      	}

		LineRenderer *lineRenderer = LineRenderer::getSingleton();
		lineRenderer->addLine(startPos, endPos, color);
		vector<LineRenderer::Line> lines = lineRenderer->getLines();

		return lines[lines.size() - 1].id;
	}

	void Player::issueOrder(Order::TYPE type, Vector3 destDir, vector<Order::Target> targets, bool append){
		vector<Unit*> selectedUnits = getSelectedUnits();

        for (Unit *u : selectedUnits) {
			bool targetingSelf = false, structBuilt = true;

			for(Order::Target &targ : targets){
				if(targ.unit && targ.unit == u){
					targetingSelf = true;
					break;
				}

				if(!u->isVehicle() && ((Structure*)u)->isComplete()){
					structBuilt = false;
					break;
				}
			}

			if(targetingSelf || !structBuilt) continue;

			int lineId = -1;
			ActiveGameState *activeState = ((ActiveGameState*)GameManager::getSingleton()->getStateManager()->getAppStateByType(AppStateType::ACTIVE_STATE));

			if(activeState && activeState->getPlayer() == this && type != Order::TYPE::EJECT)
				lineId = getOrderLineId(type, u->getPos(), targets[0].pos);

			u->receiveOrder(Order(type, targets, destDir, lineId), append);
        }
	}

	void Player::removeUnit(Unit *unit){
		for(int i = 0; i < units.size(); i++)
			if(unit == units[i]){
				removeUnit(i);
				break;
			}
	}

	void Player::removeUnit(int id){
		if(find(selectedUnits.begin(), selectedUnits.end(), units[id]) != selectedUnits.end())
			deselectUnit(units[id]);

		delete units[id];
		units.erase(units.begin() + id);

		if(cpuPlayer){
			sol::state_view SOL_LUA_VIEW = generateView();
			int id = getCpuPlayerId() + 1;
			SOL_LUA_VIEW.script("game.cpuPlayers[" + to_string(id) + "]:updateTaskForces()");
		}
	}

	void Player::removeResourceDeposit(int id){
		delete resourceDeposits[id];
		resourceDeposits.erase(resourceDeposits.begin() + id);
	}

	void Player::removeProjectile(Projectile *proj){
		for(int i = 0; i < projectiles.size(); i++)
			if(proj == projectiles[i]){
				removeProjectile(i);
				break;
			}
	}

	void Player::removeProjectile(int id){
		delete projectiles[id];
		projectiles.erase(projectiles.begin() + id);
	}

	void Player::selectUnits(vector<Unit*> selUnits){
		for(Unit *u : selUnits){
			bool garrisonable = (!u->isVehicle() || (u->isVehicle() && !((Vehicle*)u)->getGarrisonable()));
			bool selected = (find(selectedUnits.begin(), selectedUnits.end(), u) != selectedUnits.end());

			if(garrisonable && !selected){
				selectedUnits.push_back(u);
				u->select();
			}
		}
	}

	vector<Unit*> Player::getUnitsById(int id, int numUnits){
		vector<Unit*> idUnits;

		for(Unit *unit : units){
			if(numUnits != -1 && idUnits.size() == numUnits)
				break;

			if(unit->getId() == id)
				idUnits.push_back(unit);
		}

		return idUnits;
	}

	vector<Unit*> Player::getUnitsByClass(UnitClass uc, int numUnits){
		vector<Unit*> ucUnits;

		for(Unit *unit : units){
			if(numUnits != -1 && ucUnits.size() == numUnits)
				break;

			if(unit->getUnitClass() == uc)
				ucUnits.push_back(unit);
		}

		return ucUnits;
	}

	void Player::updateTradedResource(Player *player, ResourceType type, int amount, bool selfDistributed, bool increased){
		for(pair<Player*, vector<TradedResource>> pair : tradedResources)
			if(pair.first == player){
				TradedResource &tr = pair.second[(int)type];

				if(selfDistributed) (increased ? tr.taken : tr.given) += amount;
				else (increased ? tr.received : tr.hadTaken) += amount;

				break;
			}
	} 

	void Player::initTradingVecs(){
		vector<Player*> players = Game::getSingleton()->getPlayers();

		for(Player *pl : players){
			if(this == pl) continue;

			if(team == pl->getTeam()){
				tradeOffers.push_back(make_pair(pl, vector<TradeOffer*>{}));
				tradedResources.push_back(make_pair(pl, vector<TradedResource>{
							TradedResource(ResourceType::REFINEDS), 
							TradedResource(ResourceType::WEALTH), 
							TradedResource(ResourceType::RESEARCH)
				}));
			}
		}
	}

	void Player::addTradeOffer(Player *player, TradeOffer *offer){
		if(tradeOffers.empty()) initTradingVecs();

		for(pair<Player*, vector<TradeOffer*>> &offers : tradeOffers)
			if(offers.first == player)
				offers.second.push_back(offer);
	}

	vector<TradeOffer*> Player::getTradeOffers(Player *player){
		for(pair<Player*, vector<TradeOffer*>> offers : tradeOffers)
			if(offers.first == player)
				return offers.second;

		return vector<TradeOffer*>{};
	}

	void Player::deselectUnit(Unit *unit){
		for(int i = 0; i < selectedUnits.size(); i++)
			if(selectedUnits[i] == unit){
				selectedUnits.erase(selectedUnits.begin() + i);
				break;
			}
	}

	vector<GameObject*> Player::getDestructables(){
		vector<GameObject*> destructables;

		for(Projectile *p : projectiles)
			if(p->getDestructable())
				destructables.push_back(p);

		for(Unit *unit : units)
			destructables.push_back((GameObject*)unit);

		return destructables;
	}

	void Player::updateGameStats(Unit *targetUnit){
		Destructable *destructTarg = targetUnit->getDestructable();

		if(destructTarg->getHealth() <= destructTarg->getDeathHp()){
			Player *targUnitPlayer = targetUnit->getPlayer();

			if(targetUnit->isVehicle()){
				incVehiclesDestroyed();
				targUnitPlayer->incVehiclesLost();
			}
			else{
				incStructuresDestroyed();
				targUnitPlayer->incStructuresLost();
			}
		}
	}

    vector<Unit*> Player::getFriendlyUnits(bool includeOwn){
		vector<Unit*> friendlyUnits;

        for (Player *pl : Game::getSingleton()->getPlayers(true))
            for (Unit *u : pl->getUnits())
				if((includeOwn && pl == this) || pl->getTeam() == getTeam())
                	friendlyUnits.push_back(u);

		return friendlyUnits;
	}

    vector<Unit*> Player::getHostileUnits(){
		vector<Unit*> hostileUnits, friendlyUnits = getFriendlyUnits(true);

        for (Player *pl : Game::getSingleton()->getPlayers(true))
            for (Unit *u : pl->getUnits())
				if(pl->getTeam() != getTeam() && isObjectVisible(u, friendlyUnits))
                	hostileUnits.push_back(u);

		return hostileUnits;
	}

	//TODO improve this for greater accuracy
    bool Player::isObjectVisible(GameObject *object, std::vector<Unit*> friendlyUnits) {
		Player *objPlayer = object->getPlayer();

		for(Unit *friendlyUnit : friendlyUnits){
			if(objPlayer == this || objPlayer->getTeam() == getTeam())
				return true;
			
			Vector3 obsUnitPos = object->getPos();
			Vector2 oup2d = Vector2(obsUnitPos.x, obsUnitPos.z);
			
			Vector3 compUnitPos = friendlyUnit->getPos();
			Vector2 cup2d = Vector2(compUnitPos.x, compUnitPos.z);
			
			if(cup2d.getDistanceFrom(oup2d) <= friendlyUnit->getLineOfSight())
				return true;
		}

		return false;
    }

	int Player::getCpuPlayerId(){
		vector<Player*> cpuPlayers = Game::getSingleton()->getCpuPlayers();

		for(int i = 0; i < cpuPlayers.size(); i++)
			if(cpuPlayers[i] == this)
				return i;

		return -1;
	}
}
