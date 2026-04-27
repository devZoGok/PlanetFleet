#include <algorithm>
#include <quaternion.h>
#include <model.h>
#include <mesh.h>
#include <material.h>
#include <texture.h>

#include "vessel.h"
#include "util.h"
#include "vesselData.h"
#include "shell.h"
#include "guidedMissile.h"
#include "projectileData.h"

using namespace std;
using namespace vb01;

namespace battleship{
		using namespace unitData;

    Vessel::Turret::Turret(Unit *vessel, int unitId, int turretId) {
        this->vessel = vessel;
        this->unitId = unitId;
        this->turretId = turretId;

				string fr[]{DEFAULT_TEXTURE};
        Texture *turretDiffTex = new Texture(fr, 1, false);

        GameManager *gm = GameManager::getSingleton();
				Root *root = Root::getSingleton();
				string libPath = root->getLibPath();
				Material *turretMat = new Material(libPath + "texture");
				turretMat->addBoolUniform("texturingEnabled", true);
				turretMat->addBoolUniform("lightingEnabled", false);
				turretMat->addTexUniform("textures[0]", turretDiffTex, true);

        Node *hull = vessel->getNode()->getChild(0);
				node = hull->getChild(turretId);
				node->getMesh(0)->setMaterial(turretMat);
				initNodeRot = node->getOrientation();

				for(int i = 0; i < numberOfMantlets[unitId][turretId]; i++){
						Mantlet mantlet;
						mantlet.mantletNode = node->getChild(i);
						mantlet.mantletNode->getMesh(0)->setMaterial(turretMat);
						mantlet.barrelNode = mantlet.mantletNode->getChild(0);
						mantlet.barrelNode->getMesh(0)->setMaterial(turretMat);
						mantlets.push_back(mantlet);
				}

				maxAngle = unitData::turretMaxAngle[unitId][turretId] / 180 * PI;
				rotationSpeed = unitData::turretRotationSpeed[unitId][turretId] / 180 * PI;
        
        sfxBuffer = new sf::SoundBuffer();

        if(sfxBuffer->loadFromFile(getExplosionSfxPath().c_str()))
            sfx = new sf::Sound(*sfxBuffer);
    }

    void Vessel::Turret::rotate(double angle) {
        this->angle += angle;
				node->setOrientation(Quaternion(this->angle, Vector3::VEC_J) * initNodeRot);
    }

    void Vessel::Turret::update() {
        dirVec = node->getGlobalAxis(2), leftVec = node->getGlobalAxis(0);
    }

    Vector3 Vessel::Turret::getInitDir(int dirType){
				Vector3 initDirs[3];

        switch(unitData::initDirs[unitId][turretId]){
            case unitData::FORWARD:
                initDirs[0] = vessel->getLeftVec();
                initDirs[1] = vessel->getUpVec();
                initDirs[2] = vessel->getDirVec();
                break;
            case unitData::BACKWARD:
                initDirs[0] = -vessel->getLeftVec();
                initDirs[1] = vessel->getUpVec();
                initDirs[2] = -vessel->getDirVec();
                break;
            case unitData::LEFT:
                initDirs[0] = -vessel->getDirVec();
                initDirs[1] = vessel->getUpVec();
                initDirs[2] = vessel->getLeftVec();
                break;
            case unitData::RIGHT:
                initDirs[0] = vessel->getDirVec();
                initDirs[1] = vessel->getUpVec();
                initDirs[2] = -vessel->getLeftVec();
                break;
        }

				return initDirs[dirType];
    }
    
    void Vessel::Turret::fire() {
        if(sfx)
						sfx->play();

        lastShotTime = getTime();
    }

    Vessel::Vessel(Player *player, Vector3 pos, int id) : Unit(player, pos, id) {
        for (int i = 0; i < numberOfTurrets[id]; i++)
            turrets.push_back(new Turret(this, id, i));
    }

    void Vessel::update() {
        Unit::update();

        for (Turret *t : turrets) {
            t->update();

            if ((!orders.empty() && orders[0].type != Order::TYPE::ATTACK) || orders.empty()) {
                float rotAngle = abs(t->getAngle()) < t->getRotationSpeed() ? abs(t->getAngle()) : t->getRotationSpeed();
                t->rotate(t->getAngle() > 0 ? -rotAngle : rotAngle);
            }
        }
    }

    void Vessel::attack(Order order) {
        Vector3 target = order.targets[0].pos;

        for (Turret *t : turrets) {
      			Vector3 turrPos = t->getNode()->localToGlobalPosition(Vector3::VEC_ZERO);
						Vector3 targDir = (target - turrPos);
						targDir.y = 0;
						targDir = targDir.norm();
            bool right = (t->getLeftVec().getAngleBetween(targDir) > PI / 2);
            float angle = t->getInitDir(0).getAngleBetween(targDir);
            float rotAngle = (t->getRotationSpeed() > angle ? angle : t->getRotationSpeed() * (right ? -1 : 1));

            if (fabs(t->getAngle() + rotAngle) <= fabs(t->getMaxAngle())) {
                t->rotate(rotAngle);

                if (t->canFire())
										t->fire();
            }
        }
    }
}
