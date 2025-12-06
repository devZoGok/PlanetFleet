#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <vector.h>
#include <quaternion.h>
#include <model.h>

namespace sf{
	class SoundBuffer;
	class Sound;
}

namespace battleship{
	class Player;
	class Unit;

	class GameObject{
		public:
			enum class Type{UNIT, PROJECTILE, RESOURCE_DEPOSIT};

			GameObject(Type, int, Player*, vb01::Vector3, vb01::Quaternion);
			~GameObject(){}
			virtual void reinit();
			virtual void update();
        	virtual void select(){}
			virtual void placeAt(vb01::Vector3);
			void orientAt(vb01::Quaternion);
			std::string getGameObjTableName();
			void updateGameStats(Unit*);
			static sf::Sound* prepareSfx(sf::SoundBuffer*, std::string);
			bool pointWithinObj(vb01::Vector3, float = 0, float = 0, bool = false, float = 0);
			void changePlayer(Player *newPlayer);
			inline float getMaxUnevenness(){return maxUnevenness;}
        	inline vb01::Vector2 getScreenPos(){return screenPos;}
			inline vb01::Vector3 getCorner(int i){return corners[i];}
			inline bool isSelectable(){return selectable;}
			inline bool isDebuggable(){return debugging;}
			inline vb01::Vector3 getPos() {return pos;}
			inline vb01::Quaternion getRot(){return rot;}
			inline float getWidth() {return width;}
			inline float getHeight() {return height;}
			inline float getLength() {return length;}
			inline vb01::Model* getModel() {return model;}
			inline void setPlayer(Player *pl){player = pl;}
			inline Player* getPlayer(){return player;}
			inline void toggleDebugging(bool d){this->debugging=d;}
			inline vb01::Vector3 getDirVec() {return dirVec;}
			inline vb01::Vector3 getLeftVec() {return leftVec;}
			inline vb01::Vector3 getUpVec() {return upVec;}
        	inline int getId() {return id;}
			inline Type getType(){return type;}
			inline vb01::Node* getHitbox(){return hitbox;}
			inline bool isRemove(){return remove;}
		protected:
			virtual void useColor(vb01::Material*);
			virtual void initProperties();
			virtual void destroyModel();
			virtual void initModel(bool = true);
			virtual void initHitbox();
			virtual void destroyHitbox();
			virtual void destroySound();
			virtual void initSound(){}

			Type type;
			int id;
			Player *player;
			vb01::Model *model = nullptr;
			vb01::Node *hitbox = nullptr;
			vb01::Vector3 pos = vb01::Vector3(0, 0, 0), upVec = vb01::Vector3(0, 1, 0), dirVec = vb01::Vector3(0, 0, 1), leftVec = vb01::Vector3(1, 0, 0), corners[8];
			vb01::Vector2 screenPos;
			vb01::Quaternion rot;
			bool selectable = false, debugging = false, remove = false;
			float width, height, length, maxUnevenness;
	};
}

#endif
