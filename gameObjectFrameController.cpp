#include <solUtil.h>

#include "gameObjectFrameController.h"
#include "activeGameState.h"
#include "resourceDeposit.h"
#include "defConfigs.h"
#include "player.h"
#include "game.h"
#include "unit.h"
#include "util.h"
#include "map.h"

#include <material.h>
#include <meshData.h>
#include <quad.h>
#include <model.h>
#include <root.h>

#include <stateManager.h>
#include <solUtil.h>

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

	void GameObjectFrameController::paintSelect(Vector3 rowEnd){
		Vector3 rowDir = rowEnd - paintSelectRowStart;
		float lenRow = rowDir.getLength();
		float width = gameObjectFrames[0].getWidth(); 
		float length = gameObjectFrames[0].getLength();
		float hypothenuse = sqrt(width * width + length * length);
		int numStructsInRow = int(lenRow / hypothenuse);
		
		while(gameObjectFrames.size() > numStructsInRow && gameObjectFrames.size() > 1)
			removeGameObjectFrame(gameObjectFrames.size() - 1);

		if(gameObjectFrames.size() < numStructsInRow){
			int structureId = gameObjectFrames[0].getId();
			Vector3 buildDir = rowDir; 

			if(gameObjectFrames.size() > 1)
				buildDir = gameObjectFrames[1].getModel()->getPosition() - gameObjectFrames[0].getModel()->getPosition();

			Vector3 pos = paintSelectRowStart + buildDir.norm() * hypothenuse * gameObjectFrames.size();
			addGameObjectFrame(GameObjectFrame(structureId, GameObject::Type::UNIT, nullptr, pos));
		}
	}

	void GameObjectFrameController::snapToObj(GameObjectFrame &s, vector<GameObject*> snapTargets, float maxDist){
		s.status = GameObjectFrame::NOT_PLACEABLE;

		for(GameObject *st : snapTargets){
			Vector3 stPos = st->getPos();

			if(s.getPos().getDistanceFrom(stPos) < maxDist){
				s.status = GameObjectFrame::PLACEABLE;
				s.placeAt(stPos);
				checkPlacement(s);
				break;
			}
		}
	}

	//TODO fix which unit frames light green or red
	void GameObjectFrameController::checkPlacement(GameObjectFrame &s){
		Map *map = Map::getSingleton();
		MeshData meshData = map->getNodeParent()->getChild(0)->getMesh(0)->getMeshBase();
		MeshData::Vertex *verts = meshData.vertices;
		int numVerts = 3 * meshData.numTris;

		s.status = GameObjectFrame::PLACEABLE;

		if(s.getMaxUnevenness() > 0)
			for(int i = 0; i < numVerts; i++){
				float diffX = fabs(s.getPos().x - verts[i].pos->x);
				float diffY = fabs(s.getPos().y - verts[i].pos->y);
				float diffZ = fabs(s.getPos().z - verts[i].pos->z);

				if(diffX < 0.5 * s.getWidth() && diffZ < 0.5 * s.getLength() && diffY > s.getMaxUnevenness()){
					s.status = GameObjectFrame::NOT_PLACEABLE;
					break;
				}
			}

		vector<Unit*> units;

		for(Player *player : Game::getSingleton()->getPlayers(true)){
			vector<Unit*> u = player->getUnits();
			units.insert(units.end(), u.begin(), u.end());
		}

		for(Unit *unit : units){
			if(unit == s.getOriginalUnit()) continue;

			Vector3 gm1Pos = s.getPos();
			Vector3 gm1Dir = s.getDirVec();
			gm1Dir = Vector3(gm1Dir.x, 0, gm1Dir.z).norm();

			Vector3 gm1Left = s.getLeftVec();
			gm1Left = Vector3(gm1Left.x, 0, gm1Left.z).norm();

			Vector3 gm2Pos = unit->getPos();

			Vector3 gm2Dir = unit->getDirVec();
			gm2Dir = Vector3(gm2Dir.x, 0, gm2Dir.z).norm();

			Vector3 gm2Left = unit->getLeftVec();
			gm2Left = Vector3(gm2Left.x, 0, gm2Left.z).norm();

			bool intersects = rectanglesIntersect(
					Vector2(gm1Pos.x, gm1Pos.z),
					Vector2(gm1Dir.x, gm1Dir.z), 
					Vector2(gm1Left.x, gm1Left.z), 
					Vector2(s.getWidth(), s.getLength()), 
					Vector2(gm2Pos.x, gm2Pos.z),
					Vector2(gm2Dir.x, gm2Dir.z), 
					Vector2(gm2Left.x, gm2Left.z), 
					Vector2(unit->getWidth(), unit->getLength())
			);

			if(intersects){
				s.status = GameObjectFrame::NOT_PLACEABLE;
				break;
			}
		}
	}

	//TODO replace the magic number for ray casting height
	void GameObjectFrameController::shiftVerticalPlacement(){
		if(!minDepthCalculated){
			Map *map = Map::getSingleton();
			Node *nodeParent = map->getNodeParent();
			Vector3 cellSize = map->getCellSize(), waterBodyPos;
			bool inWater = false;
		
			for(int i = 1; i < nodeParent->getNumChildren(); i++){
				Vector3 wPos = nodeParent->getChild(i)->getPosition();
				Vector3 wSize = ((Quad*)nodeParent->getChild(i)->getMesh(0))->getSize();
				
				if(fabs(wPos.x - placementPos.x) < .5 * wSize.x && fabs(wPos.z - placementPos.z) < .5 * wSize.y){
					waterBodyPos = wPos;
					inWater = true;
					break;
				}
			}

			if(!inWater) return;
		
			vector<RayCaster::CollisionResult> res = map->raycastTerrain(
				Vector3(placementPos.x, 100, placementPos.z), 
				-Vector3::VEC_J,
				false
			);
		
			vector<Map::Cell> &cells = map->getCells();
			int cid = map->getCellId(placementPos, false);
			int numSubmarineCells = cells[cid].underWaterCellIds.size();
			maxDepth = cells[cid].pos.y;
			minDepth = (numSubmarineCells > 0 ? cells[cells[cid].underWaterCellIds[numSubmarineCells - 1]].pos.y : maxDepth) - .5 * cellSize.y;
			minDepthCalculated = true;
		}
		else{
			ActiveGameState *activeState = (ActiveGameState*)(GameManager::getSingleton()->getStateManager()->getAppStateByType((int)AppStateType::ACTIVE_STATE));
			float newDepth = minDepth + activeState->getDepth() * (maxDepth - minDepth);
			placementPos.y = newDepth;
		}
	}

	void GameObjectFrameController::update(){
		Map *map = Map::getSingleton();
		Vector3 startPos = Root::getSingleton()->getCamera()->getPosition();
		Vector3 endPos = screenToSpace(getCursorPos());
		vector<RayCaster::CollisionResult> results = map->raycastTerrain(startPos, (screenToSpace(getCursorPos()) - startPos).norm(), true);

		if(results.empty()) return;

		if(paintSelecting) paintSelect(results[0].pos);

		if(placingVertically) shiftVerticalPlacement();
		else if(placingOnSurface) placementPos = results[0].pos;

		vector<GameObject*> deposits;
		for(ResourceDeposit *rd : Game::getSingleton()->getCivilianPlayer()->getResourceDeposits())
			deposits.push_back((GameObject*)rd);

		for(int i = 0; i < gameObjectFrames.size(); i++){
			gameObjectFrames[i].update();

			if(paintSelecting){
				float width = gameObjectFrames[0].getWidth(); 
				float length = gameObjectFrames[0].getLength(); 
				float hypothenuse = sqrt(width * width + length * length);
				Vector3 dir = (results[0].pos - paintSelectRowStart).norm(); 
				placementPos = paintSelectRowStart + dir * hypothenuse * i;
			}

			if(!rotating && (placingOnSurface || placingVertically))
				gameObjectFrames[i].placeAt(placementPos);

			bool extractor = false;

			if(gameObjectFrames[i].getType() == GameObject::Type::UNIT){
				sol::table tbl = generateView()["units"][gameObjectFrames[i].getId() + 1];
				extractor = ((UnitClass)tbl["unitClass"] == UnitClass::EXTRACTOR);
			}

			if(extractor)
				snapToObj(gameObjectFrames[i], deposits, 20);
			else
				checkPlacement(gameObjectFrames[i]);

			bool placeable = (gameObjectFrames[i].status == GameObjectFrame::PLACEABLE);
			Vector4 color = (placeable ? Vector4(0, 1, 0, 1) : Vector4(1, 0, 0, 1));
			gameObjectFrames[i].getModel()->getMaterial()->setVec4Uniform("diffuseColor", color);
		}

		if(!placingVertically) minDepthCalculated = false;
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
