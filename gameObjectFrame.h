#ifndef GAME_OBJECT_FRAME_H
#define GAME_OBJECT_FRAME_H

#include "gameObject.h"

#include <vector.h>
#include <quaternion.h>

namespace battleship{
	struct GameObjectFrame : public GameObject{
		enum Status{PLACEABLE, NOT_PLACEABLE, PLACED};
	
		GameObjectFrame(int i, GameObject::Type t, Player *pl, Unit *ou = nullptr, vb01::Vector3 pos = vb01::Vector3::VEC_ZERO, vb01::Quaternion rot = vb01::Quaternion::QUAT_W) : 
			originalUnit(ou), 
			GameObject(t, i, pl, pos, rot)
		{
			initModel(false);
			initProperties();
			placeAt(pos);
			orientAt(rot);
		}    
		~GameObjectFrame(){}
		void destroy(){destroyModel();}
		inline Unit* getOriginalUnit(){return originalUnit;}
	
		Status status;
		Unit *originalUnit = nullptr;
	};
}

#endif
