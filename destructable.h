#ifndef DESTRUCTABLE_H
#define DESTRUCTABLE_H

#include "gameObject.h"

namespace battleship{
	enum class Armor {CAST, COMBINED, MECHANIC, SHELL, STEEL};

	class Destructable : public GameObject{
		public:
			Destructable(Player*, int, GameObject::Type, vb01::Vector3, vb01::Quaternion);
			void update();
			virtual void initProperties();
			inline int getHealth(){return health;}
			inline int getDeathHp(){return DEATH_HP;}
			inline std::vector<Armor> getArmorTypes(){return armorTypes;}
        	inline void takeDamage(int damage) {health -= damage * (1 + (freezeDmgFactor - 1) * freezeStatus * .01f);}
		private:
			std::vector<Armor> armorTypes;
        	const int DEATH_HP = 0;
		protected:
        	int health = 0, maxHealth, lenHpBar = 200, freezeStatus = 0, freezeDmgFactor = 10;
			vb01::Node *hpBackgroundNode = nullptr, *hpForegroundNode = nullptr;

			void removeBar(vb01::Node*);
			vb01::Node* createBar(vb01::Vector2, vb01::Vector2, vb01::Vector4);
    		void displayStats(vb01::Node*, vb01::Node*, int, int, bool, vb01::Vector2 offset = vb01::Vector2::VEC_ZERO);
	};
}

#endif
