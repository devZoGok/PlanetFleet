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
			void updateLuaPlayers(bool);
			void togglePause();
			void removeAllElements();
			void initTechnologies();
			float calcAbilFromTech(Ability::Type, std::vector<int>, int, int);
			bool isUnitUnlocked(std::vector<int>, int);
			std::vector<Player*> getPlayers(bool = false);
			inline void setDebug(bool d){this->debug = d;}
			inline void addPlayer(Player *pl){players.push_back(pl);}
			inline bool isDebug(){return debug;}
			inline Player* getPlayer(int id){return players[id];}
			inline void setCivilianPlayer(Player *pl){this->civilianPlayer = pl;}
			inline Player* getCivilianPlayer(){return civilianPlayer;}
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
			Player *civilianPlayer = nullptr;
	};
}

#endif
