#include "destructable.h"
#include "environment.h"
#include "player.h"
#include "game.h"

#include <root.h>
#include <node.h>
#include <quad.h>
#include <material.h>

#include <solUtil.h>

#include <vector>

using namespace std;
using namespace vb01;
using namespace gameBase;

namespace battleship{
	Destructable::Destructable(GameObject *obj) : gameObject(obj){
		initProperties();
	}

	void Destructable::initProperties(){
		int id = gameObject->getId();
		sol::state_view SOL_LUA_VIEW = generateView();
		string objType = gameObject->getGameObjTableName();
		sol::table objTable = SOL_LUA_VIEW[objType][gameObject->getId() + 1];

		vector<int> currTechs = gameObject->getPlayer()->getTechnologies();
        health += Game::getSingleton()->calcAbilFromTech(Ability::Type::HEALTH, currTechs, (int)gameObject->getType(), id);
		maxHealth = objTable["health"];

		if(health == 0) health = maxHealth;

		string tblName = "armor";
		sol::optional<sol::table> at = objTable[tblName];

		if(at != sol::nullopt){
			string varName = "numArmorTypes"; 
			SOL_LUA_VIEW.script(varName + " = #" + objType + "[" + to_string(id + 1) + "]." + tblName);
			int numArmorTypes = SOL_LUA_VIEW[varName];

			for(int i = 0; i < numArmorTypes; i++){
				Armor arm = (Armor)objTable[tblName][i + 1];
				armorTypes.push_back(arm);
			}
		}
	}

	void Destructable::update(){
        if (health <= DEATH_HP){
			gameObject->setRemove(true);

			sol::optional<sol::table> tblOpt = generateView()[gameObject->getGameObjTableName()][gameObject->getId() + 1]["deathFx"];

			if(tblOpt == sol::nullopt) return;

			Vector3 pos = gameObject->getPos();
			sol::table tbl = generateView()[gameObject->getGameObjTableName()][gameObject->getId() + 1]["deathFx"];
			FxManager::Fx *fx = FxManager::getSingleton()->initFx(tbl, gameObject->getModel(), false, pos);
			Environment::explode(fx, Environment::Detonation::EXPLOSION, pos);
		}
	}

	Node* Destructable::createBar(Vector2 pos, Vector2 size, Vector4 color){
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

    void Destructable::displayStats(Node *foreground, Node *background, int currVal, int maxVal, bool render, Vector2 offset) {
		foreground->setVisible(render);
		background->setVisible(render);

		if(render){
			Vector3 offset3d = Vector3(offset.x, offset.y, 0);
			Vector2 screenPos = gameObject->getScreenPos();

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

	void Destructable::removeBar(Node *node){
		Root::getSingleton()->getGuiNode()->dettachChild(node);
		delete node;
	}
}
