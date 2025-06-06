#include "environment.h"
#include "fxManager.h"
#include "player.h"
#include "game.h"

#include <root.h>
#include <node.h>
#include <material.h>
#include <texture.h>
#include <particleEmitter.h>

#include <SFML/Audio.hpp>

namespace battleship{
	using namespace std;
	using namespace vb01;

	static Environment *environment = nullptr;

	Environment* Environment::getSingleton(){
		if(!environment)
			environment = new Environment();

		return environment;
	}

	void Environment::explode(Vector3 pos, int damage, float radius, sf::Sound *explosionSfx){
		if(!(damage == 0 || radius == 0))
			for(Player *pl : Game::getSingleton()->getPlayers()){
				for(Unit *un : pl->getUnits()){
					float distance = un->getPos().getDistanceFrom(pos);

					if(distance < radius)
						un->takeDamage(int(damage * (1.f - distance / radius)));
				}
			}

		Root *root = Root::getSingleton();

		const int numFrames = 1;
		string p[numFrames];

		for(int i = 0; i < numFrames; i++)
			p[i] = GameManager::getSingleton()->getPath() + "Textures/Explosion/explosion07.png";

		Texture *tex = new Texture(p, numFrames, false);

		Material *mat = new Material(root->getLibPath() + "particle");
		mat->addTexUniform("tex", tex, true);

		ParticleEmitter *pe = new ParticleEmitter(1);
		pe->setMaterial(mat);
		pe->setLowLife(3);
		pe->setHighLife(3);
		pe->setSize(10 * Vector2::VEC_IJ);
		pe->setSpeed(0);

		Node *node = new Node(pos + Vector3(0, 2, 0));
		node->attachParticleEmitter(pe);
		node->lookAt(Vector3::VEC_J, Vector3::VEC_K);
		root->getRootNode()->attachChild(node);

		typedef FxManager::Fx Fx;
		typedef FxManager::Fx::Component Component;
		FxManager::getSingleton()->addFx(new Fx(vector<Component>{Component((void*)node, true, 50), Component((void*)explosionSfx, false, 2500)}));
	}
}
