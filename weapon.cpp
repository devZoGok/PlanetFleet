#include <node.h>
#include <box.h>
#include <particleEmitter.h>

#include "map.h"
#include "util.h"
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
		initNodes(weaponTable);

		sol::optional<sol::table> fireFxOpt = weaponTable["fireFx"];

		if(fireFxOpt != sol::nullopt){
			FxManager *fxManager = FxManager::getSingleton();
			fireFx = fxManager->initFx(weaponTable["fireFx"], unit->getModel(), true);

			if(fireFx) fxManager->addFx(fireFx);
		}
	}

	void Weapon::initNodes(sol::table weaponTable){
		sol::optional<sol::table> nodesTblOpt = weaponTable["nodes"];

		if(nodesTblOpt == sol::nullopt) return;

		sol::table tbl = weaponTable["nodes"];
		int numNodes = tbl.size();

		for(int i = 0; i < numNodes; i++){
			sol::table nodeTbl = weaponTable["nodes"][i + 1];
			maxFireAngle = nodeTbl["maxFireAngle"];
			rotSpeed = nodeTbl["rotationSpeed"];

			string name = nodeTbl["name"];
			nodes.push_back(unit->getModel()->findDescendant(name, true));
		}
	}

	void Weapon::initProjectileData(sol::table weaponTable){
		string projTableKey = "projectile";
		sol::optional<sol::table> proj = weaponTable[projTableKey];

		if(proj == sol::nullopt) return;

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

	Weapon::~Weapon(){
		FxManager *fm = FxManager::getSingleton();

		if(fireFx) fm->removeFx(fireFx);
	}

	//TODO replace unit pos with absolute weapon pos for withinAngle
	void Weapon::update(){
		if(unit->getCondition() != Unit::Condition::ABLE) return;

		int numOrders = unit->getNumOrders();
		int ordTp = (numOrders > 0 ? (int)unit->getOrder(0).type : -1);
		Destructable *targUnit = (ordTp != -1 ? unit->getOrder(0).targets[0].unit : nullptr);

		if(ordTp == (int)orderType){
			Vector3 targPos = (targUnit ? targUnit->getPos() : unit->getOrder(0).targets[0].pos);
			float targDist = unit->getPos().getDistanceFrom(targPos);

			bool withinRange = (minRange <= targDist && targDist <= maxRange);
			bool withinAngle = true;

			if(!nodes.empty()){
				Vector3 dirVec = nodes[0]->getGlobalAxis(2);
				withinAngle = Vector3(dirVec.x, 0, dirVec.z).norm().getAngleBetween((targPos - unit->getPos()).norm()) <= maxFireAngle;
			}

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

	void Weapon::updateTarget(Destructable *target){
		target->takeDamage(damage);

		if(target->getType() == GameObject::Type::UNIT)
			unit->updateGameStats((Unit*)target);
	}

	//TODO replace the 'laser' flag literal 
	void Weapon::fire(Order order){
		if(!canFire()) return;

		Destructable *target = order.targets[0].unit;
		Vector3 targPos = (target ? target->getPos() : order.targets[0].pos);

		if(fireFx) useFx(fireFx, targPos, true);

		if(projId == -1){
			sol::table weaponTbl = generateView()["units"][unit->getId() + 1]["weapons"][id + 1];
			FxManager *fxManager = FxManager::getSingleton();
			string fxKey = "unitHitFx";

			if(target) updateTarget(target);
			else{
				Map *map = Map::getSingleton();
				Map::Cell::Type cellType = map->getCells()[map->getCellId(targPos)].type;
				fxKey = (cellType == Map::Cell::Type::LAND ? "landHitFx" : "waterHitFx");
			}

			sol::optional<sol::table> hitFxOpt = weaponTbl[fxKey];

			if(hitFxOpt != sol::nullopt){
				sol::table fxTbl = weaponTbl[fxKey];
				int numFx = fxTbl.size();

				if(numFx > 0) useFx(fxManager->initFx(weaponTbl[fxKey], unit->getModel(), false), targPos, false);
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
	void Weapon::trackTarget(Vector3 targPos){
		if(nodes.empty()) return;

		Vector3 unitPos = unit->getPos();
		Vector3 targDir = (targPos - unitPos).norm();
		targDir = getVecToPlane(unitPos, targDir, unit->getUpVec());

		Vector3 dirVec = getVecToPlane(unitPos, nodes[0]->getGlobalAxis(2), unit->getUpVec());
		Vector3 leftVec = nodes[0]->getGlobalAxis(0);
		bool negate = (leftVec.getAngleBetween(targDir) < PI / 2);

		float angle = dirVec.getAngleBetween(targDir);
		float rotAngle = (rotSpeed < angle ? rotSpeed : angle);

		Quaternion rot = Quaternion((negate ? 1 : -1) * rotAngle, Vector3::VEC_J) * nodes[0]->getOrientation();
		nodes[0]->setOrientation(rot);
	}

	CryoGun::CryoGun(Unit *u, sol::table unitTable, int wid) : Weapon(u, unitTable, wid){}

	void CryoGun::updateTargetUnit(Unit *targetUnit){
		if(canFreeze()){
			targetUnit->setFreezeStatus(targetUnit->getFreezeStatus() + 1);
			lastFreezeTime = getTime();
		}
	}
}
