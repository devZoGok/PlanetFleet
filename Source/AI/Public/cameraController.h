#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include <vector.h>

namespace battleship{
	class CameraController{
		public:
			static CameraController* getSingleton();
			void updateCameraPosition();
			void orientCamera(vb01::Vector3, double);
			void zoomCamera(bool);
			inline bool isLookingAround(){return lookingAround;}
			inline void setLookingAround(bool l){lookingAround = l;}
		private:
			CameraController(){}
			
			bool lookingAround = false;
			int numZooms = 0;
	};
}

#endif
