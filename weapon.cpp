#include <node.h>
#include <box.h>
#include <particleEmitter.h>

#include "map.h"
#include "unit.h"
#include "util.h"
#include "weapon.h"
#include "player.h"
#include "fxManager.h"
#include "destructable.h"
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
		maxFireAngle = weaponTable["maxFireAngle"].get_or(.1);
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
			float rotSpeed = nodeTbl["rotationSpeed"];
			bool vertical = nodeTbl["vertical"];

			sol::optional<sol::table> angleConstrOpt = nodeTbl["angleConstraints"];
			float minAngle, maxAngle;

			if(angleConstrOpt != sol::nullopt){
				sol::table acTbl = nodeTbl["angleConstraints"];
				minAngle = acTbl["min"];
				maxAngle = acTbl["max"];
			}
			else{
				minAngle = 0;
				maxAngle = (vertical ? PI / 2 : 0);
			}

			string name = nodeTbl["name"];
			Node *node = unit->getModel()->findDescendant(name, true);

			components.push_back(Component(node, rotSpeed, minAngle, maxAngle, vertical));
		}

		//using the last node to determine a weapon's init direction
		Node *node = components[components.size() - 1].node;
		Vector3 p0 = unit->getModel()->globalToLocalPosition(node->localToGlobalPosition(Vector3::VEC_ZERO));
		Vector3 p1 = unit->getModel()->globalToLocalPosition(node->localToGlobalPosition(Vector3::VEC_K));
		initUnitSpaceDir = (p1 - p0).norm();
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
		GameObject *targDestruct = (ordTp != -1 ? unit->getOrder(0).targets[0].unit : nullptr);

		bool isWeaponPos = (!components.empty() && components[0].node);
		Vector3 refPos = (isWeaponPos ? components[0].node->localToGlobalPosition(Vector3::VEC_ZERO) : unit->getPos());

		if(ordTp == (int)orderType){
			Vector3 dirVec = (isWeaponPos ? components[components.size() - 1].node->getGlobalAxis(2) : unit->getDirVec());
			bool aimedAtTarget = false;

			if(targDestruct && targDestruct->getHitbox()){
				vector<RayCaster::CollisionResult> res = RayCaster::cast(refPos, dirVec, targDestruct->getHitbox());

				if(res.empty()) return;

				float targDist = refPos.getDistanceFrom(res[0].pos);
				aimedAtTarget = (minRange <= targDist && targDist <= maxRange);
			}
			else{
				Vector3 targPos = (targDestruct ? targDestruct->getPos() : unit->getOrder(0).targets[0].pos);
				float targDist = refPos.getDistanceFrom(targPos);
				bool withinRange = (minRange <= targDist && targDist <= maxRange);
				bool withinAngle = (dirVec.getAngleBetween((targPos - refPos).norm()) <= maxFireAngle);
				aimedAtTarget = withinRange && withinAngle;
			}

			if((Order::TYPE)ordTp == Order::TYPE::ATTACK && aimedAtTarget)
				fire(unit->getOrder(0));
		}
		else{
			Vector3 dir = (
					initUnitSpaceDir.x * unit->getLeftVec() + 
					initUnitSpaceDir.y * unit->getUpVec() + 
					initUnitSpaceDir.z * unit->getDirVec()
				).norm();
			trackTarget(refPos + dir);
		}
	}

	void Weapon::useFx(FxManager::Fx *fx, Vector3 targPos, bool fire){
		if(fx && !fire) FxManager::getSingleton()->addFx(fx);
		else if(!fx) return;

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

	void Weapon::updateTarget(GameObject *target){
		target->getDestructable()->takeDamage(damage);

		if(target->getType() == GameObject::Type::UNIT)
			unit->getPlayer()->updateGameStats((Unit*)target);
	}

	//TODO replace the 'laser' flag literal 
	void Weapon::fire(Order order){
		if(!canFire()) return;

		GameObject *target = order.targets[0].unit;
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
		for(Component &component : components){
			bool isWeaponPos = components[0].node;
			Vector3 weaponPos = (isWeaponPos ? components[0].node->localToGlobalPosition(Vector3::VEC_ZERO) : unit->getPos());

			Vector3 unitUp = unit->getUpVec();
			Vector3 targDir = (targPos - weaponPos).norm();
			Vector3 nodeDir = component.node->getGlobalAxis(2);

			Vector3 rotAxis;
			float rotAngle;

			if(component.vertical){
				float angle1 = unitUp.getAngleBetween(targDir);
				float angle2 = unitUp.getAngleBetween(nodeDir);
				float angleDiff = angle1 - angle2;
				rotAngle = (component.rotSpeed < fabs(angleDiff) ? component.rotSpeed : fabs(angleDiff)) * (angleDiff > 0 ? 1 : -1);

				if(PI / 2 - (angle2 + rotAngle) > component.maxAngle)
					rotAngle = PI / 2 - component.maxAngle - angle2;
				else if(PI / 2 - (angle2 + rotAngle) < component.minAngle)
					rotAngle = PI / 2 - component.minAngle - angle2;

				rotAngle = (fabs(rotAngle) > .001 ? rotAngle : 0);
				rotAxis = Vector3::VEC_I;
			}
			else{
				rotAxis = Vector3::VEC_J;
				Vector3 targDirProj = getVecToPlane(weaponPos, targDir, unitUp);
				Vector3 nodeLeft = component.node->getGlobalAxis(0);
				float angle = nodeDir.getAngleBetween(targDirProj);
				bool negate = (nodeLeft.getAngleBetween(targDirProj) > PI / 2);
				rotAngle = (negate ? -1 : 1) * (component.rotSpeed < angle ? component.rotSpeed : angle);

				if(!(component.minAngle == 0 && component.maxAngle == 0)){
					Vector3 refDir = (
						initUnitSpaceDir.x * unit->getLeftVec() + 
						initUnitSpaceDir.y * unit->getUpVec() + 
						initUnitSpaceDir.z * unit->getDirVec()
					).norm();
					float angle2 = refDir.getAngleBetween(nodeDir);

					bool dirOnLeft = (nodeLeft.getAngleBetween(refDir) > PI / 2);

					if(!dirOnLeft && -angle2 + rotAngle < component.minAngle)
						rotAngle = component.minAngle + angle2;
					else if(dirOnLeft && angle2 + rotAngle > component.maxAngle)
						rotAngle = component.maxAngle - angle2;
				}

			}

			Quaternion rot = Quaternion(rotAngle, rotAxis) * component.node->getOrientation();
			component.node->setOrientation(rot);
		}
	}
}
