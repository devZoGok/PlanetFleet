#include <solUtil.h>

#include "gameObjectFrameController.h"
#include "resourceDeposit.h"
#include "defConfigs.h"
#include "player.h"
#include "game.h"
#include "unit.h"
#include "util.h"
#include "map.h"

#include <material.h>
#include <meshData.h>
#include <mesh.h>
#include <model.h>
#include <root.h>

#include <string>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace gameBase;

	static GameObjectFrameController *gameObjectFrameController = nullptr;

	GameObjectFrameController* GameObjectFrameController::getSingleton(){
		if(!gameObjectFrameController)
			gameObjectFrameController = new GameObjectFrameController();

		return gameObjectFrameController;
	}

	void GameObjectFrameController::paintSelect(Vector3 rowEnd, float width, float length){
		Vector3 rowDir = rowEnd - paintSelectRowStart;
		float lenRow = rowDir.getLength();
		float hypothenuse = sqrt(width * width + length * length);
		int numStructsInRow = int(lenRow / hypothenuse);
		
		while(gameObjectFrames.size() > numStructsInRow)
			removeGameObjectFrame(gameObjectFrames.size() - 1);

		if(gameObjectFrames.size() < numStructsInRow){
			int structureId = gameObjectFrames[0].getId();
			Vector3 buildDir = rowDir; 

			if(gameObjectFrames.size() > 1)
				buildDir = gameObjectFrames[1].getModel()->getPosition() - gameObjectFrames[0].getModel()->getPosition();

			Vector3 pos = paintSelectRowStart + buildDir.norm() * hypothenuse * gameObjectFrames.size();
			addGameObjectFrame(GameObjectFrame(structureId, GameObject::Type::UNIT, pos));
		}
	}

	//TODO specify what to snap the frame to
	void GameObjectFrameController::snapToObj(GameObjectFrame &s, GameObject::Type type, int unitId, float maxDist){
		if(s.getType() == GameObject::Type::UNIT && s.getId() == unitId){
			s.status = GameObjectFrame::NOT_PLACEABLE;
			vector<Player*> players = Game::getSingleton()->getPlayers();
			vector<ResourceDeposit*> deposits;
		
			for(Player *pl : players){
				vector<ResourceDeposit*> deps = pl->getResourceDeposits();
		
				for(ResourceDeposit *dep : deps)
					deposits.push_back(dep);
			}

			for(ResourceDeposit *dep : deposits){
				Vector3 depPos = dep->getPos();

				if(!dep->getExtractor() && s.getPos().getDistanceFrom(depPos) < maxDist){
					s.status = GameObjectFrame::PLACEABLE;
					s.placeAt(depPos);
					break;
				}
			}
		}
	}

	//TODO implement terrain evenness check
	//TODO factor out literal values
	void GameObjectFrameController::placeGameObjectFrame(int id, Vector3 newPos, float width, float length){
		Map *map = Map::getSingleton();
		MeshData meshData = map->getNodeParent()->getChild(0)->getMesh(0)->getMeshBase();
		MeshData::Vertex *verts = meshData.vertices;
		int numVerts = 3 * meshData.numTris;

		float maxUnevenness = .5, unevenness = 0;

		for(int i = 0; i < numVerts; i++){
			float diffX = fabs(newPos.x - verts[i].pos->x);
			float diffY = fabs(newPos.y - verts[i].pos->y);
			float diffZ = fabs(newPos.z - verts[i].pos->z);

			if(diffX < 0.5 * width && diffZ < 0.5 * length && diffY > unevenness){
				unevenness = diffY;

				if(unevenness > maxUnevenness)
					break;
			}
		}

		GameObjectFrame &s = gameObjectFrames[id];
		s.placeAt(newPos);

		s.status = (unevenness < maxUnevenness ? GameObjectFrame::PLACEABLE : GameObjectFrame::NOT_PLACEABLE);
		snapToObj(s, GameObject::Type::UNIT, 20, 3);

		Material *mat = s.getModel()->getMaterial();
		Vector4 color = (s.status == GameObjectFrame::NOT_PLACEABLE ? Vector4(1, 0, 0, 1) : Vector4(0, 1, 0, 1));
		mat->setVec4Uniform("diffuseColor", color);
	}

	void GameObjectFrameController::update(){
		for(GameObjectFrame &frame : gameObjectFrames)
			frame.update();

		Vector3 startPos = Root::getSingleton()->getCamera()->getPosition();
		vector<RayCaster::CollisionResult> results = Map::getSingleton()->raycastTerrain(startPos, (screenToSpace(getCursorPos()) - startPos).norm(), true);

		if(results.empty()) return;

		Vector3 newPos, rowEnd;
		sol::table sizeTable = generateView()[gameObjectFrames[0].getGameObjTableName()][gameObjectFrames[0].getId() + 1]["size"]; 
    	float width = sizeTable["x"], length = sizeTable["z"];

		if(paintSelecting)
			paintSelect(results[0].pos, width, length);
		else if(!(paintSelecting || rotatingStructure))
			newPos = results[0].pos;

		for(int i = 0; i < gameObjectFrames.size(); i++){
			if(paintSelecting){
				float hypothenuse = sqrt(width * width + length * length);
				Vector3 dir = (results[0].pos - paintSelectRowStart).norm(); 
				newPos = paintSelectRowStart + dir * hypothenuse * i;
			}


			if(!rotatingStructure) placeGameObjectFrame(i, newPos, width, length);
		}
	}

	void GameObjectFrameController::removeGameObjectFrame(int i){
		gameObjectFrames[i].destroy();
		gameObjectFrames.erase(gameObjectFrames.begin() + i);
	}

	void GameObjectFrameController::removeGameObjectFrames(){
		while(gameObjectFrames.size() > 0)
			removeGameObjectFrame(0);
	}

	void GameObjectFrameController::rotateGameObjectFrames(float angle){
		for(GameObjectFrame &s : gameObjectFrames)
			if(s.status != GameObjectFrame::PLACED)
			   	s.orientAt(Quaternion(angle, Vector3::VEC_J) * s.getRot());
	}
}
