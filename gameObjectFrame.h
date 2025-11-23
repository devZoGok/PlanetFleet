#ifndef GAME_OBJECT_FRAME_H
#define GAME_OBJECT_FRAME_H

#include "gameObject.h"

#include <vector.h>
#include <quaternion.h>

namespace battleship{
	struct GameObjectFrame : public GameObject{
		enum Status{PLACEABLE, NOT_PLACEABLE, PLACED};
	
		GameObjectFrame(int i, GameObject::Type t, vb01::Vector3 pos = vb01::Vector3::VEC_ZERO, vb01::Quaternion rot = vb01::Quaternion::QUAT_W) : GameObject(t, i, nullptr, pos, rot) {
			initModel(false);
			initProperties();
			placeAt(pos);
			orientAt(rot);
		}    
		~GameObjectFrame(){}
		void destroy(){destroyModel();}
	
		Status status;
	};
}

#endif
