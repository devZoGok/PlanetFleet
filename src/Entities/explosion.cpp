#include <SFML/Audio.hpp>

#include "explosion.h"
#include "util.h"
#include "inGameAppState.h"
#include "game.h"

using namespace std;
using namespace vb01;

namespace battleship{
    void detonate(string p){
		sf::SoundBuffer *sfxBuffer = new sf::SoundBuffer();
		sf::Sound *sfx = nullptr;
        string  p = GameManager::getSingleton()->getPath() + "Sounds/Explosions/explosion0" + to_string(rand() % 4) + ".ogg";

        if(sfxBuffer->loadFromFile(p.c_str())){
            sfx = new sf::Sound(*sfxBuffer);
            sfx->play();
        }

		Game::getSingleton()->addFx(Fx(sfx, 2500));
    }

    void detonateDepthCharge(){
		sf::SoundBuffer *sfxBuffer = new sf::SoundBuffer();
		sf::Sound *sfx = nullptr;
        string p = GameManager::getSingleton()->getPath() + "Sounds/Destroyers/depthCharge.ogg";

        if(sfxBuffer->loadFromFile(p.c_str())){
            sfx = new sf::Sound(*sfxBuffer);
            sfx->play();
        }
				
		Game::getSingleton()->addFx(Fx(sfx, 2000));
    }

    void detonateTorpedo(){
		sf::SoundBuffer *sfxBuffer = new sf::SoundBuffer();
		sf::Sound *sfx = nullptr;
        string p = GameManager::getSingleton()->getPath() + "Sounds/Submarines/torpedo.ogg";

        if(sfxBuffer->loadFromFile(p.c_str())){
            sfx = new sf::Sound(*sfxBuffer);
            sfx->play();
        }

		Game::getSingleton()->addFx(Fx(sfx, 250));
    }
}
