#include "fxManager.h"
#include "gameObject.h"

#include <root.h>
#include <model.h>
#include <box.h>
#include <material.h>
#include <vector.h>
#include <particleEmitter.h>

#include <SFML/Audio.hpp>

namespace battleship{
	using namespace std;
	using namespace vb01;

	FxManager::Fx::Component::Component(void *c, bool v, s64 dur, vb01::Vector3 p, s64 ot) : comp(c), vfx(v), duration(dur), pos(p), offsetTime(ot) {
		if(v) ((vb01::Node*)c)->setVisible(false);
	}

	FxManager::Fx::Fx(std::vector<FxManager::Fx::Component> comps, bool re) : components(comps), reuse(re){
		toggleComponents(!re);
	}

	FxManager::Fx* FxManager::initFx(sol::table fxTbl, Node *baseNode, bool attached, Vector3 cp){
		int numComponents = fxTbl.size();

		if(numComponents == 0) return nullptr;

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
				Vector3 compPos = cp;
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

					flashNode = new Node(compPos + Vector3(0, 2, 0), compRot, Vector3(sc, sc, sc));
					flashNode->attachParticleEmitter(pe);
					flashNode->lookAt(Vector3::VEC_J, Vector3::VEC_K);
				}
				else{
					mat = new Material(Root::getSingleton()->getLibPath() + "texture");

					sol::table sizeTbl = meshTbl["size"];
					Box *box = new Box(Vector3(sizeTbl["x"], sizeTbl["y"], 0));
					box->setMaterial(mat);

					flashNode = new Node(compPos, compRot, Vector3(sc, sc, sc), "laser");
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
				Node *parNode = (attached ? baseNode : Root::getSingleton()->getRootNode());
				sol::optional<string> parNameOpt = compTbl["parent"];

				if(parNameOpt != sol::nullopt){
					string parName = compTbl["parent"];
					parNode = baseNode->findDescendant(parName, true);
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

	void FxManager::Fx::toggleComponents(bool active){
		for(Component &component : components){
			component.active = active;
			component.initTime = getTime();
		}
	}

	static FxManager *fxManager = nullptr;

	FxManager* FxManager::getSingleton(){
		if(!fxManager)
			fxManager = new FxManager();

		return fxManager;
	}

	void FxManager::update(){
		for(int i = 0; i < fxs.size(); i++){
			s64 currTime = getTime();
			Fx *fx = fxs[i];

			for(int j = 0; j < fx->components.size(); j++){
				Fx::Component &comp = fx->components[j];
				s64 currTime = getTime();

				if(comp.comp && currTime - comp.initTime > comp.offsetTime){
					if(comp.active){
						if(comp.vfx)
							((Node*)comp.comp)->setVisible(true);
						else
							((sf::Sound*)comp.comp)->play();

						comp.active = false;
						comp.initTime = getTime();
					}
					else if(!comp.active && currTime - comp.initTime > comp.duration){
						if(fx->reuse){
							if(comp.vfx)
								((Node*)comp.comp)->setVisible(false);
							else
								((sf::Sound*)comp.comp)->stop();
						}
						else{
							destroyFxComponent(i, j);
							fx->components.erase(fx->components.begin() + j);
							j--;
						}
					}
				}

			}

			if(fx->components.empty()){
				delete fx;
				fxs.erase(fxs.begin() + i);
				i--;
			}
		}
	}

	void FxManager::destroyFxComponent(int fid, int cid){
		Fx *fx = fxs[fid];

		if(fx->components[cid].vfx){
			Node *vfxNode = (Node*)fx->components[cid].comp, *parNode = vfxNode->getParent();
			parNode->dettachChild(vfxNode);
			delete vfxNode;
		}
		else{
			sf::Sound *sfx = (sf::Sound*)fx->components[cid].comp;
			const sf::SoundBuffer *buffer = sfx->getBuffer();
			sfx->stop();

			delete sfx;
			delete buffer;
		}

		fx->components[cid].comp = nullptr;
	}

	void FxManager::removeFx(Fx *fx){
		int id = -1;

		for(int i = 0; i < fxs.size(); i++)
			if(fxs[i] == fx){
				id = i;
				break;
			}

		for(int i = 0; i < fx->components.size(); i++)
			destroyFxComponent(id, i);

		fxs.erase(fxs.begin() + id);
		delete fx;
	}
}
