#include <cmath>
#include <chrono>
#include <thread>

#include <camera.h>
#include <quad.h>
#include <root.h>
#include <vector>
#include <util.h>
#include <assetManager.h>

#include <stateManager.h>
#include <solUtil.h>

#include <glm.hpp>
#include <ext.hpp>

#include <tinydir.h>

#include "util.h"
#include "defConfigs.h"
#include "gameManager.h"
#include "concreteGuiManager.h"
#include "guiAppState.h"
#include "inGameAppState.h"
#include "loadingAppState.h"
#include "mapEditorButton.h"

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;
using namespace glm;
using namespace vb01;
using namespace vb01Gui;

namespace battleship
{
	using namespace configData;

	void handleLoadingGui(LoadingAppState *loadState)
	{
		ConcreteGuiManager *guiManager = ConcreteGuiManager::getSingleton();
		guiManager->readLuaScreenScript("loadingScreen.lua");

		sol::state_view SOL_LUA_VIEW = generateView();
		string vfxPrefix = SOL_LUA_VIEW["vfxPrefix"], gameObjPrefix = SOL_LUA_VIEW["gameObjPrefix"];

		AssetManager *assetManager = AssetManager::getSingleton();
		string path = GameManager::getSingleton()->getPath();
		vector<string> assets = vector<string>{path + DEFAULT_TEXTURE};

		assetManager->readDir(path + vfxPrefix, assets, true);
		assetManager->readDir(path + gameObjPrefix, assets, true);

		loadState->setLoadableAssets(assets);

		StateManager *stateManager = GameManager::getSingleton()->getStateManager();
		stateManager->attachAppState(loadState);
	}

	Vector3 getVecToPlane(Vector3 pos, Vector3 dirVec, Vector3 upVec)
	{
		float baseAngle = upVec.getAngleBetween(dirVec.norm());

		if (fabs(baseAngle - PI / 2) > .001)
		{
			bool acuteAngle = (baseAngle < PI / 2);

			if (acuteAngle)
				baseAngle = PI / 2 - baseAngle;
			else
				baseAngle -= PI / 2;

			float pointToPlane = dirVec.getLength() * sin(baseAngle);
			return (dirVec + upVec * pointToPlane * (acuteAngle ? -1 : 1)).norm();
		}
		else
			return dirVec.norm();
	}

	vector<string> readDir(string path, bool findFolders)
	{
		tinydir_dir dir;
		tinydir_open_sorted(&dir, path.c_str());
		vector<string> files;

		for (int i = 0; i < dir.n_files; i++)
		{
			tinydir_file file;
			tinydir_readfile_n(&dir, &file, i);

			if (file.is_dir == findFolders && file.name[0] != '.')
				files.push_back(file.name);
		}

		tinydir_close(&dir);

		return files;
	}

	Vector3 spaceToScreen3d(Vector3 pos)
	{
		Root *root = Root::getSingleton();
		Camera *cam = root->getCamera();
		Vector3 dir = cam->getDirection(), up = cam->getUp();
		Vector3 camPos = cam->getPosition();
		mat4 view = lookAt(vec3(camPos.x, camPos.y, camPos.z), vec3(camPos.x + dir.x, camPos.y + dir.y, camPos.z + dir.z), vec3(up.x, up.y, up.z));

		float fov = cam->getFov(), width = root->getWidth(), height = root->getHeight(), nearPlane = cam->getNearPlane(), farPlane = cam->getFarPlane();
		mat4 proj = perspective(radians(fov), width / height, nearPlane, farPlane);

		vec4 ndcPos = proj * view * vec4(pos.x, pos.y, pos.z, 1);
		ndcPos.x /= ndcPos.w;
		ndcPos.y /= ndcPos.w;
		ndcPos.z /= ndcPos.w;
		return Vector3(0.5 * width * (1 + ndcPos.x), 0.5 * height * (1 - ndcPos.y), ndcPos.z);
	}

	Vector2 spaceToScreen(Vector3 pos)
	{
		Vector3 pos3d = spaceToScreen3d(pos);
		return Vector2(pos3d.x, pos3d.y);
	}

	Vector3 screenToSpace(Vector2 pos)
	{
		GameManager *gm = GameManager::getSingleton();
		float midWidth = .5 * gm->getWidth(), midHeight = .5 * gm->getHeight(), horOffset, vertOffset;
		bool left, up;

		if (pos.x < midWidth)
		{
			left = true;
			horOffset = pos.x / midWidth - 1;
		}
		else
		{
			left = false;
			horOffset = (pos.x - midWidth) / midWidth;
		}

		if (pos.y < midHeight)
		{
			up = true;
			vertOffset = 1 - pos.y / midHeight;
		}
		else
		{
			up = false;
			vertOffset = -(pos.y - midHeight) / midHeight;
		}

		Camera *cam = Root::getSingleton()->getCamera();
		float camNorm = cam->getNearPlane();
		float tg = tan(radians(cam->getFov()) / 2), ar = midWidth / midHeight;
		float camHeight = camNorm * tg, camWidth = camHeight * ar;
		Vector3 posOffset = (cam->getLeft() * camWidth * horOffset + cam->getUp() * camHeight * vertOffset + cam->getDirection() * camNorm);

		return cam->getPosition() + posOffset;
	}
}
