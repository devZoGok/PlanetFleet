#include "researchButton.h"
#include "activeGameState.h"
#include "researchStruct.h"
#include "game.h"

#include <stateManager.h>
#include <solUtil.h>

#include <quad.h>
#include <material.h>
#include <node.h>
#include <root.h>

#include <button.h>

#include <algorithm>

namespace battleship{
	using namespace vb01;
	using namespace std;

	ResearchButton::ResearchButton(Vector3 pos, Vector2 size, string name, int trigger, string imagePath, int tid) :
	   	UnitButton(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", trigger, imagePath), techId(tid){
			Root *root = Root::getSingleton();
			Material *mat = new Material(root->getLibPath() + "gui");
			mat->addVec4Uniform("diffuseColor", Vector4::VEC_IJKL);

			Quad *quad = new Quad(Vector3(size.x, size.y, 1), false);
			quad->setMaterial(mat);

			overlay = new Node(Vector3(pos.x, pos.y, .12));
			overlay->attachMesh(quad);
			root->getGuiNode()->attachChild(overlay);
		}

	ResearchButton::~ResearchButton(){
		Root::getSingleton()->getGuiNode()->dettachChild(overlay);
		delete overlay;
	}

	void ResearchButton::onClick(){
		if(!active) return;

		vector<Unit*> labs = getUnits();

		for(Unit *lab : labs)
			((ResearchStruct*)lab)->appendToQueue(techId);
	}

	void ResearchButton::update(){
		Button::update();

		ActiveGameState *activeState = (ActiveGameState*)(GameManager::getSingleton()->getStateManager()->getAppStateByType((int)AppStateType::ACTIVE_STATE));
		vector<int> technologies = activeState->getPlayer()->getTechnologies();
		bool hasTech = (find(technologies.begin(), technologies.end(), techId) != technologies.end());

		float alpha = .6;
		string uniform = "diffuseColor";
		active = false;

		if(!hasTech){
			vector<int> techParents = Game::getSingleton()->getTechnology(techId).parents;
			bool hasTechParents = true;

			for(int tp : techParents)
				if(find(technologies.begin(), technologies.end(), tp) == technologies.end()){
					hasTechParents = false;
					break;
				}

			if(hasTechParents){
				active = true;
				overlay->setVisible(false);

				vector<Unit*> labs = getUnits();

				for(Unit *lab : labs){
					vector<int> researchQueue = ((ResearchStruct*)lab)->getQueue();

					if(find(researchQueue.begin(), researchQueue.end(), techId) != researchQueue.end()){
						active = false;
						overlay->setVisible(true);
						overlay->getMesh(0)->getMaterial()->setVec4Uniform(uniform, Vector4(0, 0, 1, alpha));
						break;
					}
				}
			}
			else
				overlay->getMesh(0)->getMaterial()->setVec4Uniform(uniform, Vector4(1, 0, 0, alpha));
		}
		else
			overlay->getMesh(0)->getMaterial()->setVec4Uniform(uniform, Vector4(0, 0, 0, alpha));
	}
}
