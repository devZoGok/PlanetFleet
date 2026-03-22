#include "cameraController.h"
#include "defConfigs.h"

#include <quaternion.h>
#include <root.h>
#include <camera.h>

namespace battleship{
	using namespace vb01;
	using namespace configData;

	static CameraController *cameraController = nullptr;

	CameraController* CameraController::getSingleton(){
		if(!cameraController)
			cameraController = new CameraController();

		return cameraController;
	}

    void CameraController::updateCameraPosition() {
		GameManager *gm = GameManager::getSingleton();
        Vector2 cursorPos = getCursorPos();

		Camera *cam = Root::getSingleton()->getCamera();
		Vector3 camDir = cam->getDirection();
        Vector3 forwVec = Vector3(camDir.x, 0, camDir.z).norm();
		Vector3 camLeft = cam->getLeft();
		camLeft = Vector3(camLeft.x, 0, camLeft.z).norm();

		int width, height;
		glfwGetWindowSize(Root::getSingleton()->getWindow(), &width, &height);

        if (cursorPos.x <= 0 && 0 < cursorPos.y && cursorPos.y < height) 
            cam->setPosition(cam->getPosition() - camLeft * camPanSpeed);

        if (cursorPos.x >= width && 0 < cursorPos.y && cursorPos.y < height) 
            cam->setPosition(cam->getPosition() + camLeft * camPanSpeed);

        if (cursorPos.y <= 0 && 0 < cursorPos.x && cursorPos.x < width) 
            cam->setPosition(cam->getPosition() + forwVec * camPanSpeed);
				
        if (cursorPos.y >= height && 0 < cursorPos.x && cursorPos.x < width) 
            cam->setPosition(cam->getPosition() - forwVec * camPanSpeed);
    }

	void CameraController::orientCamera(Vector3 rotAxis, double str){
		float minStr = .001;
		Quaternion rotQuat = Quaternion(.5 * (fabs(str) > minStr ? str : 0), rotAxis);
		Camera *cam = Root::getSingleton()->getCamera();
		Vector3 dir = rotQuat * cam->getDirection(), up = rotQuat * cam->getUp();
		cam->lookAt(dir, up);
	}

	void CameraController::zoomCamera(bool zoomIn){
		if((zoomIn && numZooms > configData::NUM_MAX_ZOOMS) || (!zoomIn && numZooms < -configData::NUM_MAX_ZOOMS)) return;

		Vector3 newPos;
		float offset;

		if(zoomIn){
			numZooms++;
			offset = configData::CAMERA_ZOOM_INCREMENT;
		}
		else{
			numZooms--;
			offset = -configData::CAMERA_ZOOM_INCREMENT;
		}

		Camera *cam = Root::getSingleton()->getCamera();
		cam->setPosition(cam->getPosition() + cam->getDirection() * offset);
	}
}
