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
			void checkPlacement(GameObjectFrame&);
			void addGameObjectFrame(GameObjectFrame);
			inline int getNumGameObjectFrames(){return gameObjectFrames.size();}
			inline GameObjectFrame& getGameObjectFrame(int i){return gameObjectFrames[i];}
			inline bool isPaintSelecting(){return paintSelecting;}
			inline void setPaintSelecting(bool ps){paintSelecting = ps;}
			inline bool isRotating(){return rotating;}
			inline void setRotating(bool rs){rotating = rs;}
			inline bool isPlacingOnSurface(){return placingOnSurface;}
			inline void setPlacingOnSurface(bool ps){placingOnSurface = ps;}
			inline bool isPlacingVertically(){return placingVertically;}
			inline void setPlacingVertically(bool v){this->placingVertically = v;}
			inline void setPaintSelectRowStart(vb01::Vector3 st){paintSelectRowStart = st;}
			inline void toggleFrameTransformations(bool rot, bool pos, bool pv){
				setRotating(rot);
				setPlacingOnSurface(pos);
				setPlacingVertically(pv);
			}
		private:
			GameObjectFrameController(){}
			void paintSelect(vb01::Vector3);
			void snapToObj(GameObjectFrame&, std::vector<GameObject*>, float);
			void shiftVerticalPlacement();

			std::vector<GameObjectFrame> gameObjectFrames;
			vb01::Vector3 paintSelectRowStart, placementPos;
			float minDepth, maxDepth;
	   		bool paintSelecting = false, rotating = false, placingOnSurface = false, placingVertically = false, minDepthCalculated = false;
	};
}

#endif
