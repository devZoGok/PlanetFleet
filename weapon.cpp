#include <node.h>
#include <box.h>
#include <particleEmitter.h>

#include "map.h"
#include "weapon.h"
#include "player.h"
#include "fxManager.h"
#include "gameObjectFactory.h"
#include "projectile.h"

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace gameBase;

	Weapon::Weapon(Unit *u, sol::table unitTable, int wid) : 
		unit(u), 
		id(wid),
		rateOfFire(unitTable["weapons"][wid + 1]["rateOfFire"]),
		damage(unitTable["weapons"][wid + 1]["damage"].get_or(0))
	{
		sol::table weaponTable = unitTable["weapons"][wid + 1];
		maxRange = weaponTable["maxRange"];
		orderType = (Order::TYPE)weaponTable["orderType"];

		initProjectileData(weaponTable);

		sol::optional<sol::table> horNodeTblOpt = unitTable["weapons"][wid + 1]["horizontalNode"];
		sol::optional<sol::table> vertNodeTblOpt = unitTable["weapons"][wid + 1]["verticalNode"];

		if(horNodeTblOpt != sol::nullopt) horNode = initNode(weaponTable, true);
		if(vertNodeTblOpt != sol::nullopt) vertNode = initNode(weaponTable, false);

		FxManager *fxManager = FxManager::getSingleton();
		fireFx = fxManager->initFx(weaponTable["fireFx"], unit->getModel(), true);
		if(fireFx) fxManager->addFx(fireFx);
	}


	Node* Weapon::initNode(sol::table weaponTable, bool horizontal){
		sol::table nodeTbl = weaponTable[horizontal ? "horizontalNode" : "verticalNode"];
		maxFireAngle = nodeTbl["maxFireAngle"];
		rotSpeed = nodeTbl["rotationSpeed"];

		string name = nodeTbl["name"];
		return unit->getModel()->findDescendant(name, true);
	}

	void Weapon::initProjectileData(sol::table weaponTable){
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

			projPar = unit->getModel();
			sol::optional<string> parNameOpt = weaponTable[projTableKey]["parent"];

			if(parNameOpt != sol::nullopt){
				string parName = weaponTable[projTableKey]["parent"];
				projPar = unit->getModel()->findDescendant(parName, true);
			}

			sol::table posTable = weaponTable[projTableKey]["pos"];
			projPos = Vector3(posTable["x"], posTable["y"], posTable["z"]);
			sol::table rotTable = weaponTable[projTableKey]["rot"];
			projRot = Quaternion(rotTable["w"], rotTable["x"], rotTable["y"], rotTable["z"]);
		}
	}

	Weapon::~Weapon(){
		FxManager *fm = FxManager::getSingleton();

		if(fireFx) fm->removeFx(fireFx);
	}

	Vector3 Weapon::calcOrientVec(int id){
		Vector3 orientVec;

		switch(id){
			case 0:
				orientVec = unit->getDirVec();
				break;
			case 1:
				orientVec = unit->getLeftVec();
				break;
			case 2:
				orientVec = unit->getUpVec();
				break;
		}

		if(horNode) orientVec = horNode->getOrientation() * orientVec;
		if(vertNode) orientVec = vertNode->getOrientation() * orientVec;

		return orientVec;
	}

	//TODO replace unit pos with absolute weapon pos for withinAngle
	void Weapon::update(){
		if(unit->getCondition() != Unit::Condition::ABLE) return;

		int numOrders = unit->getNumOrders();
		int ordTp = (numOrders > 0 ? (int)unit->getOrder(0).type : -1);
		Unit *targUnit = (ordTp != -1 ? unit->getOrder(0).targets[0].unit : nullptr);

		if(ordTp == (int)orderType){
			Vector3 targPos = (targUnit ? targUnit->getPos() : unit->getOrder(0).targets[0].pos);
			float targDist = unit->getPos().getDistanceFrom(targPos);

			bool withinRange = (minRange <= targDist && targDist <= maxRange);

			Vector3 dirVec = calcOrientVec(0);
			bool withinAngle = (horNode ? Vector3(dirVec.x, 0, dirVec.z).norm().getAngleBetween((targPos - unit->getPos()).norm()) <= maxFireAngle : true);

			if((Order::TYPE)ordTp == Order::TYPE::ATTACK && withinRange && withinAngle)
				fire(unit->getOrder(0));
		}
		else
			trackTarget(unit->getPos() + unit->getDirVec());
	}

	void Weapon::useFx(FxManager::Fx *fx, Vector3 targPos, bool fire){
		if(fx && !fire)
			FxManager::getSingleton()->addFx(fx);
		else if(!fx)
			return;

		fx->toggleComponents(true);

		Vector3 plCol = unit->getPlayer()->getColor();

		for(int i = 0; i < fx->components.size(); i++){
			FxManager::Fx::Component &comp = fx->components[i];

			if(!comp.vfx) continue;

			if(fire && ((Node*)comp.comp)->getName() == "laser"){
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

	void Weapon::updateTargetUnit(Unit *targetUnit){
		targetUnit->takeDamage(damage);
		unit->updateGameStats(targetUnit);
	}

	//TODO replace the 'laser' flag literal 
	void Weapon::fire(Order order){
		if(!canFire()) return;

		Unit *targetUnit = order.targets[0].unit;
		Vector3 targPos = (targetUnit ? targetUnit->getPos() : order.targets[0].pos);

		if(fireFx) useFx(fireFx, targPos, true);

		if(projId == -1){
			sol::table weaponTbl = generateView()["units"][unit->getId() + 1]["weapons"][id + 1];
			FxManager *fxManager = FxManager::getSingleton();

			if(targetUnit){
				updateTargetUnit(targetUnit);
				useFx(fxManager->initFx(weaponTbl["unitHitFx"], unit->getModel(), false), targPos, false);
			}
			else{
				Map *map = Map::getSingleton();
				Map::Cell::Type cellType = map->getCells()[map->getCellId(targPos)].type;
				string fxKey = (cellType == Map::Cell::Type::LAND ? "landHitFx" : "waterHitFx");
				useFx(fxManager->initFx(weaponTbl[fxKey], unit->getModel(), false), targPos, false);
			}
		}
		else{
			Quaternion r = projPar->localToGlobalOrientation(projRot);
			Vector3 p = projPar->localToGlobalPosition(projPos);
			unit->getPlayer()->addProjectile(GameObjectFactory::createProjectile(unit, projId, p, r));
		}

		lastFireTime = getTime();
	}

	//TODO improve to allow for vertical alignment 
	void Weapon::alignNode(Vector3 targPos, Node *node, bool horizontal){
		Vector3 targDir = targPos - unit->getPos();
		Vector3 targDirHor = Vector3(targDir.x, 0, targDir.z).norm(), targDirVert;

		Vector3 dirVec = calcOrientVec(0);
		Vector3 dirVecHor = Vector3(dirVec.x, 0, dirVec.z).norm(), dirVecVert;

		Vector3 leftVec = calcOrientVec(1);
		bool negate = (Vector3(leftVec.x, 0, leftVec.z).norm().getAngleBetween(targDirHor) < PI / 2);

		float angle = dirVecHor.getAngleBetween(targDirHor);
		float rotAngle = (rotSpeed < angle ? rotSpeed : angle);

		Quaternion rot = Quaternion((negate ? 1 : -1) * rotAngle, unit->getUpVec()) * node->getOrientation();
		node->setOrientation(rot);
	}

	void Weapon::trackTarget(Vector3 targPos){
		if(horNode) alignNode(targPos, horNode, true);
		//if(vertNode) alignNode(targPos, vertNode, false);
	}

	Freezer::Freezer(Unit *u, sol::table unitTable, int wid) : Weapon(u, unitTable, wid){}

	void Freezer::updateTargetUnit(Unit *targetUnit){
		if(canFreeze()){
			targetUnit->setFreezeStatus(targetUnit->getFreezeStatus() + 1);
			lastFreezeTime = getTime();
		}
	}
}
