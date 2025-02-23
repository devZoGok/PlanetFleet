#include <solUtil.h>

#include "player.h"
#include "structure.h"
#include "projectile.h"
#include "stateManager.h"
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
		refineds(30000), 
		trader(new Trader())
	{
		colorMaterial = new Material(Root::getSingleton()->getLibPath() + "texture");
		colorMaterial->addBoolUniform("lightingEnabled", true);
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
			if(!u->isRemove())
				u->update();
			else
				removeUnit(u);
		}

		vector<Projectile*> projectiles = this->projectiles; 
		for(Projectile *proj : projectiles){
			if(!proj->isRemove())
				proj->update();
			else
				removeProjectile(proj);
		}

		for(ResourceDeposit *rd : resourceDeposits)
			rd->update();
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

				if(!u->isVehicle() && ((Structure*)u)->getBuildStatus() < 100){
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
		delete units[id];
		units.erase(units.begin() + id);
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
		for(Unit *u : selUnits)
			if(find(selectedUnits.begin(), selectedUnits.end(), u) == selectedUnits.end()){
				selectedUnits.push_back(u);
				u->select();
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

	void Player::addTechnology(int techId){
		technologies.push_back(techId);
	}

	int Player::getResource(ResourceType type){
		switch(type){
			case ResourceType::REFINEDS:
				return refineds;
			case ResourceType::WEALTH:
				return wealth;
			case ResourceType::RESEARCH:
				return research;
		}
	}

	void Player::updateResource(ResourceType type, int ammount, bool add){
		switch(type){
			case ResourceType::REFINEDS:
				refineds = (add ? refineds + ammount : ammount);
				break;
			case ResourceType::WEALTH:
				wealth = (add ? wealth + ammount : ammount);
				break;
			case ResourceType::RESEARCH:
				research = (add ? research + ammount : ammount);
				break;
		}
	}

	void Player::build(int id, Vector3 pos, Quaternion rot, bool additionalOrder){
		/*
		Unit *buildStruct = GameObjectFactory::createUnit(this, id, pos, rot);
		addUnit(buildStruct);
		issueOrder(Order::TYPE::BUILD, vector<Order::Target>{Order::Target(buildStruct)}, additionalOrder);
		*/
	}
}
