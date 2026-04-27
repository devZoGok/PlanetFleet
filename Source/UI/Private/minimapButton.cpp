#include "minimapButton.h"
#include "activeGameState.h"
#include "game.h"
#include "map.h"

#include <stateManager.h>

#include <node.h>
#include <mesh.h>
#include <material.h>
#include <imageAsset.h>
#include <assetManager.h>

#include <algorithm>
#include <vector>

namespace battleship{
	using namespace vb01;
	using namespace std;

	void MinimapButton::onClick(){
		Vector2 clickPos = getCursorPos();
		float posXRatio = (clickPos.x - pos.x) / size.x;
		float posYRatio = (clickPos.y - pos.y) / size.y;

		Camera *cam = Root::getSingleton()->getCamera();
		Vector3 mapSize = Map::getSingleton()->getMapSize();
		cam->setPosition(Vector3(mapSize.x * (-.5 + posXRatio), cam->getPosition().y, mapSize.z * (-.5 + posYRatio)));
	}
}
