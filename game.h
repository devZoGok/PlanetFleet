#ifndef GAME_H
#define GAME_H

#include "technology.h"
#include "ability.h"
#include "tradeOffer.h"

#include <vector>

namespace sf{
	class Sound;
}

namespace battleship{
	class Unit;
	class Player;
	class Projectile;

	class Game{
		public:
			static Game* getSingleton();
			void update();
			void togglePause();
			void removeAllElements();
			void explode(vb01::Vector3, int, float, sf::Sound*);
			void changeUnitPlayer(Unit*, Player*);
			void initTechnologies();
			float calcAbilFromTech(Ability::Type, std::vector<int>, int, int);
			bool isUnitUnlocked(std::vector<int>, int);
			inline void setDebug(bool d){this->debug = d;}
			inline void addPlayer(Player *pl){players.push_back(pl);}
			inline bool isDebug(){return debug;}
			inline std::vector<Player*>& getPlayers(){return players;}
			inline Player* getPlayer(int id){return players[id];}
			inline int getNumPlayers(){return players.size();}
			inline Technology getTechnology(int id){return technologies[id];}
			inline Ability getAbility(int id){return abilities[id];}
		private:
			Game(){}
			void endGame(bool);
			std::vector<int> parseTechTable(int, std::string, std::string, std::string);

			bool paused = false, ended = false, debug = false;
			std::vector<Technology> technologies;
			std::vector<Ability> abilities;
			std::vector<Player*> players;
	};
}

#endif
