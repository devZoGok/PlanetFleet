#ifndef DESTRUCTABLE_H
#define DESTRUCTABLE_H

#include <vector.h>

#include <vector>
#include <algorithm>

namespace vb01{
	class Node;
}

namespace battleship{
	enum class Armor {CAST, COMBINED, MECHANIC, SHELL, STEEL};

	class GameObject;
	/// @brief A game object that can be destroyed by having its health depleted
	class Destructable{
		public:
			Destructable(GameObject*);
			void update();
			void removeBar(vb01::Node*);
			vb01::Node* createBar(vb01::Vector2, vb01::Vector2, vb01::Vector4);
    		void displayStats(vb01::Node*, vb01::Node*, int, int, bool, vb01::Vector2 offset = vb01::Vector2::VEC_ZERO);
			inline int getHealth(){return health;}
			inline int getMaxHealth(){return maxHealth;}
			inline int getDeathHp(){return DEATH_HP;}
        	inline void takeDamage(int damage) {health -= damage * (1 + (freezeDmgFactor - 1) * freezeStatus * .01f);}
			inline GameObject* getGameObject(){return gameObject;}
			inline std::vector<Armor> getArmorTypes(){return armorTypes;}
			inline void setFreezeStatus(int fs){this->freezeStatus = std::clamp(fs, 0, 100);}
			inline int getFreezeStatus(){return freezeStatus;}
		private:
			std::vector<Armor> armorTypes;
        	const int DEATH_HP = 0;
        	int health = 0, maxHealth, freezeStatus = 0, freezeDmgFactor = 10;
			GameObject *gameObject = nullptr;

			void initProperties();
	};
}

#endif
