#include <cmath>
#include <algorithm>

#include <solUtil.h>

#include <util.h>
#include <box.h>
#include <quad.h>
#include <model.h>
#include <quaternion.h>

#include "pathfinder.h"
#include "destructable.h"
#include "defConfigs.h"
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
	}

	Vehicle::~Vehicle(){
		removeAllPathpoints();
	}

	void Vehicle::update(){
		Unit::update();

		if(garrisonable) model->setVisible(false);

		for(Node *n : debugPathPoints)
			n->setVisible(Game::getSingleton()->isDebug());
	}

	void Vehicle::halt(){
		Unit::halt();
		removeAllPathpoints();

		patrolPointId = 0;
		pursuingTarget = false;
	}

	void Vehicle::startCurrentOrder(){
		Unit::startCurrentOrder();

		switch(orders[0].type){
			case Order::TYPE::LAUNCH:
			case Order::TYPE::EJECT:
				return;
	  	}

		removeAllPathpoints();

		Vector3 targPos = (orders[0].targets[0].unit ? orders[0].targets[0].unit->getPos() : orders[0].targets[0].pos);
		preparePathpoints(orders[0], targPos);
	}

	bool Vehicle::validateGarrisonOrder(Order order){
		Unit *targUnit = (Unit*)order.targets[0].unit;

		for(GarrisonSlot slot : targUnit->getGarrisonSlots())
			if(!slot.vehicle && slot.category >= garrisonCategory)
				return true;

		return false;
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
		destinationOffset = unitTable["destinationOffset"];
		garrisonCategory = unitTable["garrisonCategory"];
	}

	void Vehicle::reinit(){
		Unit::reinit();
		initProperties();
	}

	void Vehicle::arrivedAtPathpoint(bool byPlane, float vertDist){
		bool orderHasDir = (orders[0].direction != Vector3::VEC_ZERO);
		float angleToOrderDir = dirVec.getAngleBetween(orders[0].direction);
		bool destDirWithin = (!orderHasDir || (orderHasDir && angleToOrderDir <= anglePrecision));

		if(pathPoints.size() == 1 && !destDirWithin)
			turn(calculateRotation(orders[0].direction, angleToOrderDir, maxTurnAngle));

		if(byPlane && (
					(pathPoints.size() > 1 || (pathPoints.size() == 1 && destDirWithin)) && 
					(type != UnitType::UNDERWATER || (type == UnitType::UNDERWATER && vertDist < 0.5 * height))
				)
		){
			removePathpoint();
		}
		else if(!byPlane && (pathPoints.size() > 1 || (pathPoints.size() == 1 && destDirWithin)))
			removePathpoint();
	}

	void Vehicle::moveByTerrainQuads(Vector3 hypVec, float destOffset){
		Map *map = Map::getSingleton();
		Quad *terrQuad = (Quad*)map->getNodeParent()->getChild(0)->getMesh(0);
		int numVertDiv = configData::NUM_SUBDIVS, numHorDiv = configData::NUM_SUBDIVS;
		Vector3 mapSize = map->getMapSize();
		Vector2 sqIdsVec = terrQuad->getSubquadIds(numVertDiv, numHorDiv, pos, mapSize);
		Vector2 sqIdsEndVec = terrQuad->getSubquadIds(numVertDiv, numHorDiv, pathPoints[0], mapSize);

		int sqIds[]{sqIdsVec.x, sqIdsVec.y};
		vector<Vector3> points = vector<Vector3>{pos};

		// y = ax + b
		float a = hypVec.z / hypVec.x;
		float b = pos.z - a * pos.x;

		while(!(sqIds[0] == (int)sqIdsEndVec.x && sqIds[1] == (int)sqIdsEndVec.y)){
			Vector3 c1 = terrQuad->getSubquadCorner(sqIds[0], sqIds[1], numVertDiv, numHorDiv, true, false); //top left
			Vector3 c2 = terrQuad->getSubquadCorner(sqIds[0], sqIds[1], numVertDiv, numHorDiv, true, true); //top right
			Vector3 c3 = terrQuad->getSubquadCorner(sqIds[0], sqIds[1], numVertDiv, numHorDiv, false, false); //bottom left
			Vector3 c4 = terrQuad->getSubquadCorner(sqIds[0], sqIds[1], numVertDiv, numHorDiv, false, true); //bottom right

			float minX = c1.x, maxX = c2.x, minY = c1.z, maxY = c3.z;
			bool bottom = (hypVec.z > 0), left = (hypVec.x < 0);
			float intersecX = (left ? c1.x : c2.x);
			float intersecY = (bottom ? c3.z : c1.z);

			float xSolY = a * intersecX + b;
			float ySolX = (hypVec.x != 0 ? (intersecY - b) / a : points[points.size() - 1].x);

			float diff, vertDiff, x, y, z;

			if(minY <= xSolY && xSolY <= maxY){
				diff = (xSolY - c1.z) / (c3.z - c1.z);
				vertDiff = (left ? c3.y - c1.y : c4.y - c2.y);

				x = (left ? c1.x : c2.x);
				y = (left ? c1.y : c2.y) + vertDiff * diff;
				z = xSolY;

				sqIds[0] += (left ? -1 : 1);
			}
			else if(hypVec.x == 0 || (minX <= ySolX && ySolX <= maxX)){
				diff = (ySolX - c1.x) / (c2.x - c1.x);
				vertDiff = (bottom ? c4.y - c3.y : c2.y - c1.y);

				x = ySolX;
				y = (bottom ? c3.y : c1.y) + vertDiff * diff;
				z = (bottom ? c3.z : c1.z);

				sqIds[1] += (bottom ? 1 : -1);
			}

			points.push_back(Vector3(x, y, z));
		}
		
		points.push_back(pathPoints[0]);

		float movementAmmount = speed, totalDist = 0, eps = .01;
		Vector3 endPos = pos;

		for(int i = 0; i < points.size() - 1; i++){
			Vector3 diffVec = (points[i + 1] - points[i]).norm();
			float dist = points[i].getDistanceFrom(points[i + 1]);
			float diff = dist;

			if(fabs(totalDist + dist - movementAmmount) > eps)
				diff = movementAmmount - totalDist;

			endPos += diffVec * diff;
			totalDist += diff;
		}

		placeAt(endPos);

		if(fabs(pathPoints[0].x - pos.x) <= destOffset && fabs(pathPoints[0].z - pos.z) <= destOffset)
			arrivedAtPathpoint(false);
	}

	void Vehicle::moveByPlane(Vector3 hypVec, float destOffset){
		Vector3 linDest = Vector3(pathPoints[0].x, pos.y, pathPoints[0].z);

		if(pos.getDistanceFrom(linDest) > destOffset){
			float dist = pos.getDistanceFrom(linDest);
			float movementAmmount = (speed > dist ? dist : speed);
			advance(movementAmmount);
		}

		float vertDist = fabs(pos.y - pathPoints[0].y);

		if(vertDist > .1){
			float dist = pos.y - pathPoints[0].y;
			float movementAmmount = (speed > fabs(dist) ? dist : speed);

			if(dist > 0) movementAmmount *= -1;

			advance(movementAmmount, MoveDir::UP);
		}

		if(pos.getDistanceFrom(linDest) <= destOffset)
			arrivedAtPathpoint(true, vertDist);
	}

	void Vehicle::navigate(float destOffset){
		if(pathPoints.empty()) return;

		Vector3 hypVec = (pathPoints[0] - pos);
		Vector3 baseDir = getVecToPlane(pos, hypVec, upVec);
		if(baseDir == Vector3::VEC_ZERO) baseDir = dirVec;

		float angle = baseDir.getAngleBetween(dirVec);

		if(angle > anglePrecision && pos.getDistanceFrom(pathPoints[0]) > destOffset)
			turn(calculateRotation(baseDir, angle, maxTurnAngle));
		else if(type == UnitType::LAND)
			moveByTerrainQuads(hypVec, destOffset);
		else
			moveByPlane(hypVec, destOffset);
	}

    void Vehicle::move(Order order) {
		navigate(0.5 * Map::getSingleton()->getCellSize().x);

		if(pathPoints.empty())
			removeOrder(0);
    }

	void Vehicle::exitGarrisonable(Vector3 exitPos){
		placeAt(exitPos);
		garrisonable->updateGarrison(this, false);
		garrisonable = nullptr;
	}

	void Vehicle::enterGarrisonable(){
		player->deselectUnit(this);
		Unit *targUnit = (Unit*)orders[0].targets[0].unit; 
		targUnit->updateGarrison(this, true);

		removeAllPathpoints();
		removeOrder(0);

		garrisonable = targUnit;
		pursuingTarget = false;
	}

	void Vehicle::navigateToTarget(float minDist){
		if(!pursuingTarget){
			Vector3 targPos = (orders[0].targets[0].unit ? orders[0].targets[0].unit->getPos() : orders[0].targets[0].pos);
			preparePathpoints(orders[0], targPos, true);
			pursuingTarget = true;
		}

		navigate(minDist);
	}

	void Vehicle::garrison(Order order){
		Unit *targUnit = (Unit*)order.targets[0].unit;
		float distToGarrisonable = pos.getDistanceFrom(targUnit->getPos()), garrisonDist = Map::getSingleton()->getCellSize().x;

		if(distToGarrisonable > garrisonDist)
			navigateToTarget(garrisonDist);
		else enterGarrisonable();
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
		b->setMaterial(player->getColorMaterial());

		Node *n = new Node(pointPos);
		n->attachMesh(b);
		Root::getSingleton()->getRootNode()->attachChild(n);
		debugPathPoints.push_back(n);
	}

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

		int origDest = map->getCellId(destPos), dest = origDest;

		if(type == UnitType::SEA_LEVEL && fabs(destPos.y - cells[origDest].pos.y) > .1) return;

		Pathfinder *pf = Pathfinder::getSingleton();

		if(type != UnitType::HOVER)
			dest = pf->clampDestToSourceRegion(source, origDest);

		GameObject *targObj = order.targets[0].unit;
		Unit *blockingUnit = cells[dest].blockedBy;
		bool garrisonOrder = (order.type == Order::TYPE::GARRISON);
		bool buildOrder = (order.type == Order::TYPE::BUILD);
		bool resourceOrder = (order.type == Order::TYPE::LOAD || order.type == Order::TYPE::UNLOAD || order.type == Order::TYPE::SUPPLY);

		if(blockingUnit && blockingUnit != this && !resourceOrder && !buildOrder && !garrisonOrder){
			vector<int> surrCellIds = map->getSurroundingCells(cells[dest].pos, 1);
			int altDest = -1;

			for(int scid : surrCellIds){
				if(!cells[scid].blockedBy){
					switch(type){
						case UnitType::HOVER:
							altDest = scid;
							break;
						case UnitType::LAND:
							if(cells[scid].type == Map::Cell::LAND)
								altDest = scid;

							break;
						case UnitType::SEA_LEVEL:
						case UnitType::UNDERWATER:
							if(cells[scid].type == Map::Cell::WATER)
								altDest = scid;

							break;
					}

					if(altDest != -1){
						dest = altDest;
						break;
					}
				}
			}

			if(altDest == -1) return;
		}

		vector<float> heurs;
		vector<int> path = pf->findPath(cells, heurs, source, dest, (int)type);
		Vector3 *truncPoint = nullptr;

		if(targObj && origDest == dest && !garrisonOrder)
			for(int i = path.size() - 1; i >= 0; i--){
				Vector3 targPos = targObj->getPos();
				Vector3 pointDir = cells[path[i]].pos - targPos;
				pointDir = Vector3(pointDir.x, 0, pointDir.z);

				float dirAngle = targObj->getDirVec().getAngleBetween(pointDir.norm());
				if(dirAngle > PI / 2) dirAngle = PI - dirAngle;

				float pointDist = pointDir.getLength();

				float length = targObj->getLength(); 
				float lengthComp = pointDist * cos(dirAngle);
				bool withinLength = (.5 * length >= lengthComp);

				float width = targObj->getWidth();
				float widthComp = pointDist * sin(dirAngle);
				bool withinWidth = (.5 * width >= widthComp);

				if(!(withinLength && withinWidth)){
					float a1 = atan((.5 * width) / (.5 * length));
					float a2 = atan((.5 * length) / (.5 * width));

					float leftAngle = targObj->getLeftVec().getAngleBetween(pointDir.norm());
					if(leftAngle > PI / 2) leftAngle = PI - leftAngle;

					float dist;

					if(dirAngle <= a1) dist = (.5 * length) / cos(dirAngle);
					else if(leftAngle <= a2) dist = (.5 * width) / cos(leftAngle);

					truncPoint = new Vector3;
					*truncPoint = (targPos + pointDir.norm() * dist);

					break;
				}
				else path.pop_back();
			}

		for(int p : path) addPathpoint(cells[p].pos);

		if(appendDestPos && !truncPoint && origDest == dest)
			addPathpoint(destPos);
		else if(truncPoint){
			addPathpoint(*truncPoint);
			delete truncPoint;
		}

		path.erase(path.begin());
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

		if(pathPoints.empty()) pursuingTarget = false;
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
		float angleToTarg = dirVec.getAngleBetween(targVec.norm());

		vector<Weapon*> attackWeapons = getWeaponsByOrder(Order::TYPE::ATTACK);
		Weapon *weapon = attackWeapons[0];

		for(Weapon *w : attackWeapons)
			if(w->getMaxRange() > weapon->getMaxRange())
				weapon = w;

		float minDist = weapon->getMaxRange();
		float minAngle = weapon->getMaxFireAngle();

		if(order.playerAssigned || (!order.playerAssigned && state == Unit::State::CHASE)){
			bool horizontal = false;

			for(const Weapon::Component &comp : weapon->getComponents())
				if(!comp.vertical){
					horizontal = true;
					break;
				}

			if(distToTarg > minDist || (!horizontal && angleToTarg > minAngle))
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
		Structure *structure = (Structure*)order.targets[0].unit;
		float offset = .5 * Map::getSingleton()->getCellSize().x;

		if(pathPoints.empty() && closeEnough(structure, pos, offset)){
			sol::table targTable = generateView()["units"][structure->getId()];
			int costRate = (int)targTable["cost"] / 100, buildRate = (int)targTable["buildTime"] / 100;

			if(!structure->isComplete() && player->getResource(ResourceType::REFINEDS) >= costRate && getTime() - lastBuildTime > buildRate){
				structure->incrementBuildStatus();
				player->updateResource(ResourceType::REFINEDS, -costRate, true);
				lastBuildTime = getTime();
			}
			else if(structure->isComplete()){
				removeOrder(0);
				player->incStructuresBuilt();
			}
		}
		else navigate(offset);
	}

	void Vehicle::select(){
		if(!garrisonable)
			Unit::select();
	}

	bool Vehicle::closeEnough(GameObject *obj, Vector3 pos, float eps){
		Vector3 neVec = pos - obj->getPos();
		float angle = obj->getDirVec().getAngleBetween(neVec.norm());
	
		if(angle > PI / 2) angle = PI - angle;
	
		return cos(angle) * neVec.getLength() < .5 * obj->getLength() + eps;
	};
}
