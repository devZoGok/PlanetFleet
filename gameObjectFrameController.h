#ifndef GAME_OBJECT_FRAME_CONTROLLER_H
#define GAME_OBJECT_FRAME_CONTROLLER_H

#include <vector>
#include <string>

#include <vector.h>
#include <quaternion.h>

#include "gameObjectFrame.h"

namespace vb01{
	class Model;
}

namespace battleship{
	class GameObjectFrameController{
		public:
			static GameObjectFrameController* getSingleton();
			void update();
			void removeGameObjectFrame(int);
			void removeGameObjectFrames();
			void rotateGameObjectFrames(float);
			void placeGameObjectFrame(int, vb01::Vector3, float, float);
			inline int getNumGameObjectFrames(){return gameObjectFrames.size();}
			inline void addGameObjectFrame(GameObjectFrame u){gameObjectFrames.push_back(u);}
			inline GameObjectFrame& getGameObjectFrame(int i){return gameObjectFrames[i];}
			inline bool isPlacingFrames(){return placingStructures;}
			inline void setPlacingFrames(bool ps){placingStructures = ps;}
			inline bool isPaintSelecting(){return paintSelecting;}
			inline void setPaintSelecting(bool ps){paintSelecting = ps;}
			inline bool isRotatingFrames(){return rotatingStructure;}
			inline void setRotatingFrames(bool rs){rotatingStructure = rs;}
			inline void setPaintSelectRowStart(vb01::Vector3 st){paintSelectRowStart = st;}
		private:
			GameObjectFrameController(){}
			void paintSelect(vb01::Vector3, float, float);
			void snapToObj(GameObjectFrame&, GameObject::Type, int, float);

			std::vector<GameObjectFrame> gameObjectFrames;
			vb01::Vector3 paintSelectRowStart, rowDir;
	   		bool placingStructures = false, paintSelecting = false, rotatingStructure = false;
	};
}

#endif
