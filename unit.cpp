#include <box.h>
#include <quad.h>
#include <model.h>
#include <light.h>
#include <texture.h>
#include <material.h>
#include <particleEmitter.h>

#include <stateManager.h>
#include <solUtil.h>

#include <glm.hpp>
#include <ext.hpp>

#include "unit.h"
#include "util.h"
#include "game.h"
#include "vehicle.h"
#include "gameObjectFactory.h"
#include "activeGameState.h"
#include "gameManager.h"
#include "projectile.h"
#include "defConfigs.h"
#include "pathfinder.h"
#include "ability.h"

using namespace glm;
using namespace vb01;
using namespace gameBase;
using namespace std;

namespace battleship{
	string Unit::Weapon::LASER_FLAG = "laser";

	Unit::Weapon::Weapon(Unit *u, sol::table unitTable, int wid) : 
		unit(u), 
		id(wid),
		type((Weapon::Type)unitTable["weapons"][wid + 1]["type"]), 
		rateOfFire(unitTable["weapons"][wid + 1]["rateOfFire"]),
		damage(unitTable["weapons"][wid + 1]["damage"].get_or(0))
	{
		sol::table weaponTable = unitTable["weapons"][wid + 1];
		maxRange = weaponTable["maxRange"];
		initProjectileData(weaponTable);

		fireFx = initFx(weaponTable, "fireFx", true);
		if(fireFx) FxManager::getSingleton()->addFx(fireFx);
	}

	void Unit::Weapon::initProjectileData(sol::table weaponTable){
		string projTableKey = "projectile";
		sol::optional<sol::table> proj = weaponTable[projTableKey];

		if(proj != sol::nullopt){
			projId = weaponTable[projTableKey]["id"];
			sol::table projTable = generateView()["projectiles"];
			ProjectileClass pc = (ProjectileClass)projTable[projId + 1]["projectileClass"];

			if(pc == ProjectileClass::CRUISE_MISSILE){
				minRange = 0;
				float rotAngle = projTable[projId + 1]["rotAngle"];
				float speed = projTable[projId + 1]["speed"];
				float base = PI / 2, alpha = 0;

				while(base - alpha > .001){
					minRange += speed * sin(alpha);
					alpha += (base - alpha > rotAngle ? rotAngle : base - alpha);
				}

				minRange *= 2;
			}

			sol::table posTable = weaponTable[projTableKey]["pos"];
			projPos = Vector3(posTable["x"], posTable["y"], posTable["z"]);
			sol::table rotTable = weaponTable[projTableKey]["rot"];
			projRot = Quaternion(rotTable["w"], rotTable["x"], rotTable["y"], rotTable["z"]);
		}
	}

	//TODO implement node child search by name
	FxManager::Fx* Unit::Weapon::initFx(sol::table weaponTable, string fxKey, bool attached){
		sol::optional<sol::table> fxOpt = weaponTable[fxKey];

		if(fxOpt == sol::nullopt)
			return nullptr;

		sol::table fxTbl = weaponTable[fxKey];
		int numComponents = fxTbl.size();

		if(numComponents == 0)
			return nullptr;

		vector<FxManager::Fx::Component> fxComponents;

		for(int i = 0; i < numComponents; i++){
			sol::table compTbl = fxTbl[i + 1];

			bool vfx = compTbl["vfx"];
			s64 duration = compTbl["duration"], offset = compTbl["offset"].get_or(0);

			if(vfx){
				sol::table meshTbl = compTbl["mesh"];

				string meshPath = "";
				Material *mat = nullptr;
				Node *flashNode = nullptr;

				sol::optional<sol::table> posOpt = compTbl["pos"], rotOpt = compTbl["rot"];
				Vector3 compPos = Vector3::VEC_ZERO;
				Quaternion compRot = Quaternion::QUAT_W;
				float sc = compTbl["scale"].get_or(1);

				if(posOpt != sol::nullopt){
					sol::table posTable = compTbl["pos"];
					compPos = Vector3(posTable["x"], posTable["y"], posTable["z"]);
				}

				if(rotOpt != sol::nullopt){
					sol::table rotTable = compTbl["rot"];
					compRot = Quaternion(rotTable["w"], rotTable["x"], rotTable["y"], rotTable["z"]);
				}

				sol::optional<string> pathOpt = meshTbl["path"]; 
				sol::optional<int> numPartOpt = meshTbl["numParticles"]; 

				if(pathOpt != sol::nullopt){
					mat = new Material(Root::getSingleton()->getLibPath() + "texture");

					meshPath = meshTbl["path"];
					flashNode = new Model(meshPath);
					((Model*)flashNode)->setMaterial(mat);
					flashNode->setPosition(compPos);
					flashNode->setOrientation(compRot);
					flashNode->setScale(Vector3(sc, sc, sc));
				}
				else if(numPartOpt != sol::nullopt){
					mat = new Material(Root::getSingleton()->getLibPath() + "particle");

					int numParticles = meshTbl["numParticles"];
					ParticleEmitter *pe = new ParticleEmitter(numParticles);
					pe->setMaterial(mat);
					pe->setLowLife(meshTbl["lowLife"]);
					pe->setHighLife(meshTbl["highLife"]);
					pe->setSpeed(0);

					sol::table sizeTbl = meshTbl["size"];
					pe->setSize(Vector2(sizeTbl["x"], sizeTbl["y"]));

					flashNode = new Node(compPos, compRot, Vector3(sc, sc, sc));
					flashNode->attachParticleEmitter(pe);
					flashNode->lookAt(Vector3::VEC_J, Vector3::VEC_K);
				}
				else{
					mat = new Material(Root::getSingleton()->getLibPath() + "texture");

					sol::table sizeTbl = meshTbl["size"];
					Box *box = new Box(Vector3(sizeTbl["x"], sizeTbl["y"], 0));
					box->setMaterial(mat);

					flashNode = new Node(compPos, compRot, Vector3(sc, sc, sc), LASER_FLAG);
					flashNode->attachMesh(box);
				}

				sol::optional<string> texOpt = meshTbl["texture"];

				if(texOpt != sol::nullopt){
					string p[]{meshTbl["texture"]};
					Texture *tex = new Texture(p, 1, false);
					mat->addBoolUniform("texturingEnabled", true);
					mat->addTexUniform("diffuseMap[0]", tex, false);
				}
				else{
					sol::table colorTable = meshTbl["color"];
					mat->addVec4Uniform("diffuseColor", Vector4(colorTable["x"], colorTable["y"], colorTable["z"], colorTable["a"]));
					mat->addBoolUniform("texturingEnabled", false);
				}

				flashNode->setVisible(false);
				Node *parNode = (attached ? unit->getModel() : Root::getSingleton()->getRootNode());
				sol::optional<string> parOpt = compTbl["parent"];

				if(parOpt != sol::nullopt){
					vector<Node*> descendants;
					unit->getModel()->getDescendants(descendants);
					string parName = compTbl["parent"];
					parNode = nullptr;

					for(Node *desc : descendants)
						if(desc->getName() == parName){
							parNode = desc;
							break;
						}
				}

				parNode->attachChild(flashNode);

				fxComponents.push_back(FxManager::Fx::Component((void*)flashNode, vfx, duration, compPos, offset));
			}
			else{
				sf::SoundBuffer *sfxBuffer = new sf::SoundBuffer();
				sf::Sound *sfx = GameObject::prepareSfx(sfxBuffer, compTbl["path"]);
				fxComponents.push_back(FxManager::Fx::Component((void*)sfx, vfx, duration, Vector3::VEC_ZERO, offset));
			}
		}

		return new FxManager::Fx(fxComponents, attached);
	}

	Unit::Weapon::~Weapon(){
		FxManager *fm = FxManager::getSingleton();

		if(fireFx) fm->removeFx(fireFx);
	}

	void Unit::Weapon::update(){}

	void Unit::Weapon::useFx(FxManager::Fx *fx, Vector3 targPos, bool fire){
		if(fx && !fire)
			FxManager::getSingleton()->addFx(fx);
		else if(!fx)
			return;

		fx->toggleComponents(true);

		Vector3 plCol = unit->getPlayer()->getColor();

		for(int i = 0; i < fx->components.size(); i++){
			FxManager::Fx::Component &comp = fx->components[i];

			if(comp.vfx){
				if(fire && ((Node*)comp.comp)->getName() == LASER_FLAG){
					Vector3 initPos = unit->getPos(), laserDir = targPos - initPos;
					float targDist = laserDir.getLength();
					float angle = unit->getDirVec().getAngleBetween(laserDir);
					Vector3 crossProd = unit->getDirVec().cross(laserDir);

					Node *compNode = (Node*)comp.comp;
					compNode->setOrientation(Quaternion(angle, crossProd) * compNode->getOrientation());

					Box *box = (Box*)compNode->getMesh(0);
					Vector3 size = box->getSize();
					box->setSize(Vector3(size.x, size.y, targDist));
					box->updateVerts(box->getMeshBase());
					box->getMaterial()->setVec4Uniform("diffuseColor", Vector4(plCol.x, plCol.y, plCol.z, 1));

					compNode->setPosition(comp.pos + .5 * targDist * Vector3::VEC_K);
				}
				else if(!fire)
					((Node*)comp.comp)->setPosition(targPos);
			}
		}
	}

	void Unit::Weapon::fire(Order order){
		if(!canFire()) return;

		Unit *targetUnit = order.targets[0].unit;
		Vector3 targPos = (targetUnit ? targetUnit->getPos() : order.targets[0].pos);

		if(fireFx) useFx(fireFx, targPos, true);

		if(projId == -1){
			sol::table weaponTbl = generateView()["units"][unit->getId() + 1]["weapons"][id + 1];

			if(targetUnit){
				targetUnit->takeDamage(damage);
				unit->updateGameStats(targetUnit);
				useFx(initFx(weaponTbl, "unitHitFx", false), targPos, false);
			}
			else{
				Map *map = Map::getSingleton();
				Map::Cell::Type cellType = map->getCells()[map->getCellId(targPos)].type;
				string fxKey = (cellType == Map::Cell::Type::LAND ? "landHitFx" : "waterHitFx");
				useFx(initFx(weaponTbl, fxKey, false), targPos, false);
			}
		}
		else{
			Vector3 leftVec = unit->getLeftVec();
			Vector3 upVec = unit->getUpVec();
			Vector3 dirVec = unit->getDirVec();
			Vector3 p = unit->getPos() + leftVec * projPos.x + upVec * projPos.y + dirVec * projPos.z;
			Quaternion r = unit->getRot() * projRot;
			unit->getPlayer()->addProjectile(GameObjectFactory::createProjectile(unit, projId, p, r));
		}

		lastFireTime = getTime();
	}

    Unit::Unit(Player *player, int id, Vector3 pos, Quaternion rot, State st) : GameObject(GameObject::Type::UNIT, id, player, pos, rot), state(st){
		selectable = true;

		Unit::initProperties();
		initModel();
		initHitbox();
		initSound();
		initWeapons();
        placeAt(pos);
		orientAt(rot);

		Vector2 size = Vector2(lenHpBar, 10);
		hpBackgroundNode = createBar(Vector2::VEC_ZERO, size, Vector4(0, 0, 0, 1));
		hpForegroundNode = createBar(Vector2::VEC_ZERO, size, Vector4(0, 1, 0, 1));
    }

    Unit::~Unit() {
		removeBar(hpBackgroundNode);
		removeBar(hpForegroundNode);
		destroyWeapons();
		destroySound();
		destroyHitbox();
		destroyModel();
    }

	void Unit::initProperties(){
		GameObject::initProperties();

		sol::state_view SOL_LUA_VIEW = generateView();
		string objType = GameObject::getGameObjTableName();
		sol::table unitTable = SOL_LUA_VIEW[objType][id + 1];

		Game *game = Game::getSingleton();
		vector<int> currTechs = player->getTechnologies();

		string name = unitTable["name"];
        vehicle = unitTable["isVehicle"];
        health += game->calcAbilFromTech(Ability::Type::HEALTH, currTechs, (int)GameObject::type, id);
		maxHealth = unitTable["health"];

		if(health == 0) health = maxHealth;

        lineOfSight = unitTable["lineOfSight"]; lineOfSight += game->calcAbilFromTech(Ability::Type::LINE_OF_SIGHT, currTechs, (int)GameObject::type, id);
        unitClass = (UnitClass)unitTable["unitClass"];
		type = (UnitType)unitTable["unitType"];

		guiScreen = "";
		string gsk = "guiScreen";
		sol::optional<string> nameOpt = unitTable[gsk];

		if(nameOpt != sol::nullopt) guiScreen = unitTable[gsk];

		string tblName = "garrisonCapacity";
		sol::optional<sol::table> gc = unitTable[tblName];

		if(gc != sol::nullopt){
			string varName = "numGarrisonSlots";
			SOL_LUA_VIEW.script(varName + " = #" + objType + "[" + to_string(id + 1) + "]." + tblName);
			int numGarrisonSlots = SOL_LUA_VIEW[varName];

			for(int i = 0; i < numGarrisonSlots; i++){
				Vector2 size = 10 * Vector2::VEC_IJ;
				Vector2 pos = Vector2(1.5 * size.x * i, 20);
				Node *bg = createBar(pos, size, Vector4(0, 0, 0, 1));
				Node *fg = createBar(pos, size, Vector4(0, 1, 0, 1));
				int category = unitTable[tblName][i + 1];
				garrisonSlots.push_back(GarrisonSlot(bg, fg, pos, category));
			}
		}

		tblName = "armor";
		sol::optional<sol::table> at = unitTable[tblName];

		if(at != sol::nullopt){
			string varName = "numArmorTypes"; 
			SOL_LUA_VIEW.script(varName + " = #" + objType + "[" + to_string(id + 1) + "]." + tblName);
			int numArmorTypes = SOL_LUA_VIEW[varName];

			for(int i = 0; i < numArmorTypes; i++){
				Armor arm = (Armor)unitTable[tblName][i + 1];
				armorTypes.push_back(arm);
			}
		}

		tblName = "buildableUnits";
		sol::optional<sol::table> bu = unitTable[tblName];

		if(bu != sol::nullopt){
			SOL_LUA_VIEW.script("numBuildableUnits = #units[" + to_string(id + 1) + "]." + tblName);
			int numBuildableUnits = SOL_LUA_VIEW["numBuildableUnits"];

			for(int i = 0; i < numBuildableUnits; i++){
				sol::table buTable = unitTable[tblName][i + 1];
				buildableUnits.push_back(BuildableUnit(buTable["id"], game->isUnitUnlocked(currTechs, id) | (bool)buTable["buildable"]));
			}
		}
	}

	void Unit::initWeapons(){
		sol::state_view SOL_STATE_VIEW = generateView();
		string objType = GameObject::getGameObjTableName();
		sol::table unitTable = SOL_STATE_VIEW[objType][id + 1];

		string tblName = "weapons";
		sol::optional<sol::table> wt = unitTable[tblName];

		if(wt != sol::nullopt){
			string varName = "numWeapons";
			SOL_STATE_VIEW.script(varName + " = #" + objType + "[" + to_string(id + 1) + "]." + tblName);
			int numWeapons = SOL_STATE_VIEW[varName];

			for(int i = 0; i < numWeapons; i++)
				weapons.push_back(new Weapon(this, unitTable, i));
		}
	}

	void Unit::destroyWeapons(){
		for(Weapon *weapon : weapons)
			delete weapon;

		weapons.clear();
	}

	void Unit::initLosLight(){
		Light *light = new Light(Light::Type::POINT);
		light->setAttenuation(Light::Attenuation::NONE);
		light->setRadius(lineOfSight);
		light->setColor(Vector3::VEC_IJK * .3);
		light->setAdditiveLighting(false);
		light->setUseAngle(false);

		losLightNode = new Node(Vector3::VEC_J * 5);
		losLightNode->addLight(light);
		model->attachChild(losLightNode);
	}

	void Unit::destroyLosLight(){
		model->dettachChild(losLightNode);
		delete losLightNode;
		losLightNode = nullptr;
	}
	
	void Unit::destroySound(){
		selectionSfx->stop();
		delete selectionSfx;
		delete selectionSfxBuffer;
	}

	//TODO factor out initSound()
	void Unit::initSound(){
		GameObject::initSound();
		selectionSfxBuffer = new sf::SoundBuffer();
		string sfxPath = generateView()[getGameObjTableName()][id + 1]["selectionSfx"];
		selectionSfx = GameObject::prepareSfx(selectionSfxBuffer, sfxPath);
	}

	void Unit::reinit(){
		if(losLightNode)
			destroyLosLight();

		destroyWeapons();
		destroyHitbox();
		destroyModel();
		destroySound();

		Unit::initProperties();

		initModel();
		initHitbox();
		initSound();
		initWeapons();

		GameObject::reinit();
	}

	bool Unit::canGarrison(Vehicle *vehicle){
		for(int i = 0; i < garrisonSlots.size(); i++)
			if(!garrisonSlots[i].vehicle && garrisonSlots[i].category >= vehicle->getGarrisonCategory())
				return true;

		return false;
	}

	Node* Unit::createBar(Vector2 pos, Vector2 size, Vector4 color){
		Root *root = Root::getSingleton();
		Material *mat = new Material(root->getLibPath() + "gui");
		mat->addBoolUniform("texturingEnabled", false);
		mat->addVec4Uniform("diffuseColor", color);

		Quad *quad = new Quad(Vector3(size.x, size.y, 0), false);
		quad->setMaterial(mat);

		Node *node = new Node(Vector3(pos.x, pos.y, 0));
		node->attachMesh(quad);
		node->setVisible(false);
		root->getGuiNode()->attachChild(node);

		return node;
	}

	void Unit::removeBar(Node *node){
		Root::getSingleton()->getGuiNode()->dettachChild(node);
		delete node;
	}

	void Unit::launch(Order order){
		for(Weapon *weapon : weapons){
			Vector3 targPos = Vector3(order.targets[0].pos.x, pos.y, order.targets[0].pos.z);

			if(weapon->getProjectileId() == 0 && weapon->getMinRange() < targPos.getDistanceFrom(pos)){
				weapon->fire(order);
				break;
			}
		}

		removeOrder(0);
	}

	float Unit::calculateRotation(Vector3 dir, float angle, float maxTurnAngle){
		float rotSpeed = (maxTurnAngle > angle ? angle : maxTurnAngle); 

		if(isTargetToTheRight(dir, leftVec))
			rotSpeed *= -1;

		return rotSpeed;
	}


	void Unit::initUnitStats(){
	}

	void Unit::renderOrderLine(bool mainPlayerSelecting){
        if (!orders.empty() && orders[0].lineId != -1 && mainPlayerSelecting){
			bool display = canDisplayOrderLine();
			LineRenderer *lineRenderer = LineRenderer::getSingleton();
			lineRenderer->toggleVisibility(orders[0].lineId, display);

			if(display){
				lineRenderer->changeLineField(orders[0].lineId, pos, LineRenderer::START);
				Vector3 targPos = (orders[0].targets[0].unit ? orders[0].targets[0].unit->getPos() : orders[0].targets[0].pos);
				lineRenderer->changeLineField(orders[0].lineId, targPos, LineRenderer::END);
			}
        }
	}

	void Unit::targetUnitsAutomatically(){
		vector<Player*> players = Game::getSingleton()->getPlayers();
		vector<Unit*> units;

		for(Player *pl : players)
			if(!(pl == player || pl->getTeam() == player->getTeam())){
				vector<Unit*> un = pl->getUnits();
				units.insert(units.end(), un.begin(), un.end());
			}

		if(orders.empty())
			for(Unit *unit : units)
				if(unit->getPos().getDistanceFrom(pos) < lineOfSight){
					receiveOrder(Order(Order::TYPE::ATTACK, vector<Order::Target>{Order::Target(unit)}, Vector3::VEC_ZERO, -1, false), false);
					break;
				}
	}

    void Unit::update() {
		GameObject::update();

		ActiveGameState *activeState = (ActiveGameState*)GameManager::getSingleton()->getStateManager()->getAppStateByType(AppStateType::ACTIVE_STATE);
		Player *mainPlayer = (activeState ? activeState->getPlayer() : nullptr);

		if(state != State::HOLD_FIRE)
			targetUnitsAutomatically();

		vector<Player*> selectingPlayers = getSelectingPlayers();
		bool mainPlayerSelecting = (find(selectingPlayers.begin(), selectingPlayers.end(), mainPlayer) != selectingPlayers.end());
		bool mainPlayerOwner = (player == mainPlayer);
		bool renderSelectables = (mainPlayerSelecting && mainPlayerOwner);

		renderOrderLine(renderSelectables);
        executeOrders();

		displayUnitStats(hpForegroundNode, hpBackgroundNode, health, maxHealth, mainPlayerSelecting);

		for(GarrisonSlot &slot : garrisonSlots)
			displayUnitStats(slot.foreground, slot.background, (int)((bool)slot.vehicle), (int)true, renderSelectables, slot.offset);

        if (health <= DEATH_HP){
			Game::getSingleton()->explode(pos, 0, 0, deathSfx);
			remove = true;
		}

		for(Weapon *weapon : weapons)
			weapon->update();
    }

    void Unit::displayUnitStats(Node *foreground, Node *background, int currVal, int maxVal, bool render, Vector2 offset) {
		foreground->setVisible(render);
		background->setVisible(render);

		if(render){
			Vector3 offset3d = Vector3(offset.x, offset.y, 0);

			Quad *bgQuad = (Quad*)background->getMesh(0);
			Vector3 size = bgQuad->getSize();
			float shiftedX = screenPos.x - 0.5 * size.x;
			background->setPosition(Vector3(shiftedX, screenPos.y, 0) + offset3d);

			Quad *fgQuad = (Quad*)foreground->getMesh(0);
			fgQuad->setSize(Vector3((float)currVal / maxVal * size.x, size.y, 0));
			fgQuad->updateVerts(fgQuad->getMeshBase());
			foreground->setPosition(Vector3(shiftedX, screenPos.y, .01) + offset3d);
		}
    }

	//TODO remove order argument from action methods
    void Unit::executeOrders() {
        if (orders.size() > 0) {
            Order order = orders[0];

            switch (order.type) {
                case Order::TYPE::ATTACK:
                    attack(order);
                    break;
                case Order::TYPE::BUILD:
                    build(order);
                    break;
                case Order::TYPE::MOVE:
                    move(order);
                    break;
				case Order::TYPE::GARRISON:
					garrison(order);
					break;
				case Order::TYPE::EJECT:
					eject(order);
					break;
                case Order::TYPE::PATROL:
                    patrol(order);
                    break;
                case Order::TYPE::LAUNCH:
                    launch(order);
                    break;
                case Order::TYPE::SUPPLY:
                    supply(order);
                    break;
                case Order::TYPE::HACK:
                    hack(order);
                    break;
                default:
                    break;
            }
        }
    }

	void Unit::eject(Order order){
		Map *map = Map::getSingleton();
		int currCellId = map->getCellId(pos);
		vector<Map::Cell> &cells = map->getCells(); 
		int adjWaterCellId = -1, adjLandCellId = -1;

		for(Map::Edge edge : cells[currCellId].edges){
			if(adjLandCellId == -1 && cells[edge.destCellId].type == Map::Cell::LAND)
				adjLandCellId = edge.destCellId;
			if(adjWaterCellId == -1 && cells[edge.destCellId].type == Map::Cell::WATER)
				adjWaterCellId = edge.destCellId;
		}

		for(GarrisonSlot &slot : garrisonSlots)
			if(slot.vehicle){
				bool exitToLandCell = (slot.vehicle->getType() == UnitType::LAND && adjLandCellId != -1);
				bool exitToWaterCell = ((slot.vehicle->getType() == UnitType::SEA_LEVEL || slot.vehicle->getType() == UnitType::UNDERWATER) && adjWaterCellId != -1);
				
				if(exitToLandCell || exitToWaterCell)
					slot.vehicle->exitGarrisonable(cells[exitToLandCell ? adjLandCellId : adjWaterCellId].pos);
			}

		removeOrder(0);
	}

	void Unit::attack(Order order){
		vector<Unit*> units;

		for(Player *pl : Game::getSingleton()->getPlayers()){
			vector<Unit*> un = pl->getUnits();
			units.insert(units.end(), un.begin(), un.end());
		}

		for(Order::Target &target : orders[0].targets)
			if(target.unit){
				if(find(units.begin(), units.end(), target.unit) != units.end())
					break;

				removeOrder(0);
			}
	}

	bool Unit::validateOrder(Order order){
		switch (order.type) {
		    case Order::TYPE::ATTACK:
				return !weapons.empty();
		    case Order::TYPE::BUILD:
				return unitClass == UnitClass::ENGINEER;
		    case Order::TYPE::PATROL:
		    case Order::TYPE::MOVE:
				return vehicle;
			case Order::TYPE::GARRISON:
				return true;
			case Order::TYPE::EJECT:
				return !garrisonSlots.empty();
		    case Order::TYPE::LAUNCH:
				{
					for(Weapon *weapon : weapons)
						if(weapon->getType() == Weapon::Type::CRUISE_MISSILE)
							return true;

					return false;
				}
		    case Order::TYPE::SUPPLY:
				return unitClass == UnitClass::RESOURCE_ROVER;
		    case Order::TYPE::HACK:
				{
					for(Weapon *weapon : weapons)
						if(weapon->getType() == Weapon::Type::CRUISE_MISSILE)
							return true;

					return false;
				}
			default:
				return false;
		}
	}

    void Unit::receiveOrder(Order order, bool add) {
		if(!validateOrder(order)) return;

		if(!add){
			while (!orders.empty())
				removeOrder(0);
			
			orderLineDispTime = getTime();
		}

		orders.push_back(order);
    }

    void Unit::halt() {
        while (orders.size() > 0)
            removeOrder(orders.size() - 1);
    }

	void Unit::updateGarrison(Vehicle *garrVeh, bool entering){
		for(GarrisonSlot &slot : garrisonSlots){
			if(entering && !slot.vehicle){
				slot.vehicle = garrVeh;
				break;
			}
			else if(!entering && slot.vehicle == garrVeh){
				slot.vehicle = nullptr;
				break;
			}
		}
	}

	vector<Player*> Unit::getSelectingPlayers(){
		vector<Player*> players = Game::getSingleton()->getPlayers(), selectingPlayers;

		for(Player *pl : players){
			int numSelectedUnits = pl->getNumSelectedUnits();

			for(int i = 0; i < numSelectedUnits; i++)
				if(pl->getSelectedUnit(i) == this){
					selectingPlayers.push_back(pl);
					break;
				}
		}

		return selectingPlayers;
	}
    
    void Unit::removeOrder(int id) {
		if(orders[id].lineId != -1)
			LineRenderer::getSingleton()->removeLine(orders[id].lineId);

        orders.erase(orders.begin() + id);
    }

    void Unit::select() {
		ActiveGameState *activeState = (ActiveGameState*)GameManager::getSingleton()->getStateManager()->getAppStateByType(AppStateType::ACTIVE_STATE);
		Player *mainPlayer = (activeState ? activeState->getPlayer() : nullptr);

		vector<Player*> selectingPlayers = getSelectingPlayers();
		bool mainPlayerSelecting = (activeState && find(selectingPlayers.begin(), selectingPlayers.end(), mainPlayer) != selectingPlayers.end());

        if(mainPlayer == player && mainPlayerSelecting && selectionSfx)
            selectionSfx->play();

        orderLineDispTime = getTime();
    }
}
