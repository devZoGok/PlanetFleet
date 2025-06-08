#include <cmath>

#include <solUtil.h>

#include <util.h>
#include <box.h>
#include <model.h>
#include <quaternion.h>

#include "pathfinder.h"
#include "structure.h"
#include "vehicle.h"
#include "weapon.h"
#include "player.h"
#include "game.h"
#include "map.h"

using namespace gameBase;
using namespace vb01;
using namespace std;

namespace battleship{
	Vehicle::Vehicle(Player *player, int id, Vector3 pos, Quaternion rot, Unit::State state) : Unit(player, id, pos, rot, state){
		initProperties();

		debugMat = new Material(Root::getSingleton()->getLibPath() + "texture");
		debugMat->addBoolUniform("lightingEnabled", false);
		debugMat->addBoolUniform("texturingEnabled", false);
		debugMat->addVec4Uniform("diffuseColor", Vector4::VEC_IJKL);
	}

	Vehicle::~Vehicle(){
		removeAllPathpoints();
		delete debugMat;
	}

	void Vehicle::update(){
		Unit::update();

		if(garrisonable) model->setVisible(false);
	}

	void Vehicle::halt(){
		Unit::halt();
		removeAllPathpoints();

		patrolPointId = 0;
		pursuingTarget = false;
	}

	bool Vehicle::validateGarrisonOrder(Order order){
		Unit *targUnit = order.targets[0].unit;

		for(GarrisonSlot slot : targUnit->getGarrisonSlots())
			if(!slot.vehicle && slot.category >= garrisonCategory)
				return true;

		return false;
	}

	void Vehicle::receiveOrder(Order order, bool add){
		if(!(order.type == Order::TYPE::EJECT || order.type == Order::TYPE::LAUNCH)){
			Order::Target targ = order.targets[0];
			Vector3 targPos = (targ.unit ? targ.unit->getPos() : targ.pos);
			preparePathpoints(order, targPos);

			if(!pathPoints.empty())
				Unit::receiveOrder(order, add);
		}
		else
			Unit::receiveOrder(order, add);
	}

    void Vehicle::turn(float angle) {
        Quaternion newRot = Quaternion(angle, upVec) * model->getOrientation();
        model->setOrientation(newRot);
		rot = newRot;
    }

	void Vehicle::advance(float speed, MoveDir moveDir) {
		Vector3 dir;
		
		switch(moveDir){
			case MoveDir::FORW:
				dir = dirVec;
				break;
			case MoveDir::LEFT:
				dir = leftVec;
				break;
			case MoveDir::UP:
				dir = upVec;
				break;
		}
		
		placeAt(pos + dir * speed);
	}

	void Vehicle::initProperties(){
		Game *game = Game::getSingleton();
		vector<int> currTechs = player->getTechnologies();

		sol::table unitTable = generateView()[GameObject::getGameObjTableName()][id + 1];
        maxTurnAngle =  unitTable["maxTurnAngle"]; maxTurnAngle += game->calcAbilFromTech(Ability::Type::MAX_TURN_ANGLE, currTechs, (int)GameObject::type, id);
        speed = unitTable["speed"]; speed += game->calcAbilFromTech(Ability::Type::SPEED, currTechs, (int)GameObject::type, id);
		anglePrecision = unitTable["anglePrecision"];
		garrisonCategory = unitTable["garrisonCategory"];
	}

	void Vehicle::reinit(){
		Unit::reinit();
		initProperties();
	}

	void Vehicle::navigate(float destOffset){
		Vector3 hypVec = (pathPoints[0] - pos);
		float hypAngle = hypVec.norm().getAngleBetween(upVec) - PI / 2;
		float offset = hypVec.getLength() * sin(hypAngle);

		Vector3 linDest = pathPoints[0] + upVec * offset;
		float vertDist = fabs(pos.y - pathPoints[0].y);
		Vector3 destDir = (linDest - pos).norm();
		float angle = (destDir != Vector3::VEC_ZERO ? dirVec.getAngleBetween(destDir) : -1);

		if(angle > anglePrecision && pos.getDistanceFrom(linDest) > destOffset)
			turn(calculateRotation(destDir, angle, maxTurnAngle));
		else{
			if(pos.getDistanceFrom(linDest) > destOffset){
				float dist = pos.getDistanceFrom(linDest);
				float movementAmmount = (speed > dist ? dist : speed);
				advance(movementAmmount);
			}

			if(vertDist > .1){
				float dist = pos.y - pathPoints[0].y;
				float movementAmmount = (speed > fabs(dist) ? dist : speed);

				if(dist > 0)
					movementAmmount *= -1;

				advance(movementAmmount, MoveDir::UP);
			}

			if(pos.getDistanceFrom(linDest) <= destOffset){
				bool orderHasDir = (orders[0].direction != Vector3::VEC_ZERO);
				float angleToOrderDir = dirVec.getAngleBetween(orders[0].direction);
				bool destDirWithin = (!orderHasDir || (orderHasDir && angleToOrderDir <= anglePrecision));

				if(pathPoints.size() == 1 && !destDirWithin)
					turn(calculateRotation(orders[0].direction, angleToOrderDir, maxTurnAngle));

				if(pathPoints.size() > 1 || (pathPoints.size() == 1 && destDirWithin)){
					if(type == UnitType::UNDERWATER && vertDist < 0.5 * height)
						removePathpoint();
					else if(type != UnitType::UNDERWATER)
						removePathpoint();
				}
			}
		}
	}

	void Vehicle::alignToSurface(){
		/*
		Map *map = Map::getSingleton();
		TerrainObject terr = map->getTerrainObject(0);
		vector<RayCaster::CollisionResult> res = RayCaster::cast(Vector3(pos.x, terr.size.y, pos.z), Vector3(0, -1, 0), terr.node);
		
		if(!res.empty()){
			placeAt(res[0].pos);
		
			float angle = upVec.getAngleBetween(res[0].norm);
			Vector3 axis = upVec.cross(res[0].norm).norm();
			Quaternion rotQuat = Quaternion(angle, axis);
			orientAt(rotQuat * rot);
		}
		*/
	}

    void Vehicle::move(Order order) {
		navigate(0.5 * Map::getSingleton()->getCellSize().x);

		if(type == UnitType::LAND)
			alignToSurface();

		if(pathPoints.empty())
			removeOrder(0);
    }

	void Vehicle::exitGarrisonable(Vector3 exitPos){
		placeAt(exitPos);
		garrisonable->updateGarrison(this, false);
		garrisonable = nullptr;
	}

	void Vehicle::enterGarrisonable(){
		Unit *targUnit = orders[0].targets[0].unit; 
		targUnit->updateGarrison(this, true);

		removeAllPathpoints();
		removeOrder(0);

		garrisonable = targUnit;
		pursuingTarget = false;
	}

	void Vehicle::navigateToTarget(float minDist){
		if(!pursuingTarget){
			Vector3 targPos = (orders[0].targets[0].unit ? orders[0].targets[0].unit->getPos() : orders[0].targets[0].pos);
			preparePathpoints(orders[0], targPos);
			pursuingTarget = true;

			if(orders[0].type == Order::TYPE::GARRISON) addPathpoint(targPos);
		}

		navigate(minDist);
	}

	void Vehicle::garrison(Order order){
		Unit *targUnit = order.targets[0].unit;
		float distToGarrisonable = pos.getDistanceFrom(targUnit->getPos()), garrisonDist = Map::getSingleton()->getCellSize().x;

		if(distToGarrisonable > garrisonDist)
			navigateToTarget(garrisonDist);
		else
			enterGarrisonable();
	}

	void Vehicle::patrol(Order order){
		if(pathPoints.empty()){
			patrolPointId = getNextPatrolPointId(order.targets.size());
			preparePathpoints(order, order.targets[patrolPointId].pos);
		}

		navigate(.5 * Map::getSingleton()->getCellSize().x);
	}

	void Vehicle::addPathpoint(Vector3 pointPos){
		pathPoints.push_back(pointPos);

		Box *b = new Box(Vector3::VEC_IJK);
		b->setMaterial(debugMat);

		Node *n = new Node(pointPos);
		n->attachMesh(b);

		Root::getSingleton()->getRootNode()->attachChild(n);

		debugPathPoints.push_back(n);
	}

	//TODO allow ships to attack land targets and vice versa 
	//TODO recursively search for vacant dest cell neibourghss 
	void Vehicle::preparePathpoints(Order &order, Vector3 destPos, bool appendDestPos){
		removeAllPathpoints();

		Map *map = Map::getSingleton();
		vector<Map::Cell> &cells = map->getCells();

		int source = map->getCellId(pos);
		bool ship = (type == UnitType::UNDERWATER || type == UnitType::SEA_LEVEL);
		bool waterVehCanMove = (ship && cells[source].type == Map::Cell::WATER);
		bool landVehCanMove = (type == UnitType::LAND && cells[source].type == Map::Cell::LAND);

		if(type != UnitType::HOVER && !(waterVehCanMove || landVehCanMove)) return;

		int dest = map->getCellId(destPos);

		if(type != UnitType::UNDERWATER && fabs(destPos.y - cells[dest].pos.y) > .1) return;

		if(cells[dest].blockedBy){
			vector<int> surrCellIds = map->getSurroundingCells(cells[dest].pos, 1);

			for(int scid : surrCellIds)
				if(!cells[scid].blockedBy){
					dest = scid;
					break;
				}
		}

		vector<float> heuristics;

		for(Map::Cell &cell : cells)
			heuristics.push_back(145 * (cells[dest].pos.getDistanceFrom(cell.pos)));

		vector<int> path = Pathfinder::getSingleton()->findPath(cells, heuristics, source, dest, this);
		bool pathTruncated = false;

		for(int i = 0; i < path.size(); i++){
			if((ship && cells[path[i]].type != Map::Cell::WATER) || (order.type != Order::TYPE::GARRISON && type == UnitType::LAND && cells[path[i]].type != Map::Cell::LAND)){
				path = vector<int>(path.begin(), path.begin() + i);
				order.targets[0].unit = nullptr;
				order.targets[0].pos = cells[path[i - 1]].pos;
				pathTruncated = true;
				break;
			}
			else if(order.type == Order::TYPE::GARRISON && type == UnitType::LAND && cells[path[i]].type != Map::Cell::LAND && path.size() - 1 != i)
				return;
		}

		for(int p : path)
			addPathpoint(cells[p].pos);

		if(appendDestPos && !pathTruncated)
			addPathpoint(destPos);
	}

	void Vehicle::removePathpoint(int i){
		Node *rootNode = Root::getSingleton()->getRootNode();
		Node *debugPathPointNode = debugPathPoints[i];
		rootNode->dettachChild(debugPathPointNode);

		Mesh *mesh = debugPathPointNode->getMesh(0);
		mesh->setMaterial(nullptr);
		debugPathPoints.erase(debugPathPoints.begin() + i);
		delete debugPathPointNode;

		pathPoints.erase(pathPoints.begin() + i);

		if(pathPoints.empty())
			pursuingTarget = false;
	}

	void Vehicle::removeAllPathpoints(){
		while(!pathPoints.empty())
			removePathpoint();
	}

	void Vehicle::attack(Order order){
		int prevNumOrders = orders.size();
		Unit::attack(order);
		int currNumOrders = orders.size();

		if(prevNumOrders != currNumOrders) return;

		Order::Target target = order.targets[0];
		Vector3 targVec = (target.unit ? target.unit->getPos() : target.pos) - pos;
		float distToTarg = targVec.getLength();

		vector<Weapon*> attackWeapons = getWeaponsByOrder(Order::TYPE::ATTACK);
		Weapon *weapon = attackWeapons[0];

		for(Weapon *w : attackWeapons)
			if(w->getMaxRange() > weapon->getMaxRange())
				weapon = w;

		float minDist = weapon->getMaxRange();

		if(order.playerAssigned || (!order.playerAssigned && state == Unit::State::CHASE)){
			if(distToTarg > minDist)
				navigateToTarget(.5 * Map::getSingleton()->getCellSize().x);
			else
				pursuingTarget = false;
		}
		else if(!order.playerAssigned && state == Unit::State::STAND_GROUND && distToTarg > minDist){
			removeOrder(0);
			return;
		}
	}

	void Vehicle::build(Order order){
		if(pathPoints.empty()){
			if(!order.targets[0].unit)
				player->addUnit(order.targets[0].unit);
			else {
				Structure *structure = (Structure*)order.targets[0].unit;
				sol::table targTable = generateView()["units"][structure->getId()];
				int costRate = (int)targTable["cost"] / 100, buildRate = (int)targTable["buildTime"] / 100;

				if(structure->getBuildStatus() < 100 && player->getResource(ResourceType::REFINEDS) >= costRate && getTime() - lastBuildTime > buildRate){
					structure->incrementBuildStatus();
					player->updateResource(ResourceType::REFINEDS, -costRate, true);
					lastBuildTime = getTime();
				}
				else if(structure->getBuildStatus() >= 100){
					removeOrder(0);
					player->incStructuresBuilt();
				}
			}
		}
		else{
			navigate(0.5 * Map::getSingleton()->getCellSize().x);

			if(type == UnitType::LAND)
				alignToSurface();
		}
	}

	void Vehicle::select(){
		if(!garrisonable)
			Unit::select();
	}
}
