#include <root.h>
#include <quaternion.h>
#include <model.h>
#include <material.h>
#include <texture.h>
#include <quad.h>
#include <box.h>
#include <text.h>
#include <assetManager.h>
#include <imageAsset.h>

#include <stateManager.h>
#include <solUtil.h>

#include <algorithm>
#include <ctype.h>
#include <stdlib.h>
#include <map>

#include "activeGameState.h"
#include "inGameAppState.h"
#include "defConfigs.h"
#include "structure.h"
#include "game.h"
#include "util.h"
#include "tooltip.h"
#include "gameObjectFrameController.h"
#include "gameObjectFactory.h"
#include "cameraController.h"
#include "concreteGuiManager.h"

using namespace vb01;
using namespace vb01Gui;
using namespace std;

namespace battleship{
	using namespace configData;
	using namespace gameBase;

    ActiveGameState::ActiveGameState(GuiAppState *gs, int plId) : AbstractAppState(
						AppStateType::ACTIVE_STATE,
					 	configData::calcSumBinds(AppStateType::ACTIVE_STATE, true),
					 	configData::calcSumBinds(AppStateType::ACTIVE_STATE, false),
					 	GameManager::getSingleton()->getPath() + scripts[(int)ScriptFiles::OPTIONS]), guiState(gs), playerId(plId), depth(1){
		initDragbox();

		mainPlayer = Game::getSingleton()->getPlayer(playerId);
    }

    ActiveGameState::~ActiveGameState() {
		removeDragbox();
    }

	void ActiveGameState::initDragbox(){
		Root *root = Root::getSingleton();

		Material *mat = new Material(root->getLibPath() + "gui");
		mat->addBoolUniform("texturingEnabled", false);
		mat->addBoolUniform("diffuseColorEnabled", false);
		mat->addVec4Uniform("diffuseColor", Vector4(.5, .5, .5, .5));

		Quad *dragbox = new Quad(Vector3::VEC_ZERO, false);
		dragbox->setMaterial(mat);
		dragboxNode = new Node();
		dragboxNode->attachMesh(dragbox);

		root->getGuiNode()->attachChild(dragboxNode);
	}

	void ActiveGameState::initCursor(){
		string basePath = GameManager::getSingleton()->getPath() + "Textures/Icons/Cursors/";
		sol::state_view SOL_LUA_VIEW = generateView();

		string pt = SOL_LUA_VIEW["pointerTex"], 
			   at = SOL_LUA_VIEW["attackTex"], 
			   gt = SOL_LUA_VIEW["garrisonTex"],
			   p1[]{basePath + pt}, 
			   p2[]{basePath + at}, 
			   p3[]{basePath + gt};
		pointerTex = new Texture(p1, 1, false);
		attackTex = new Texture(p2, 1, false);
		garrisonTex = new Texture(p3, 1, false);

		Root *root = Root::getSingleton();
		Material *mat = new Material(root->getLibPath() + "gui");
		mat->addBoolUniform("texturingEnabled", true);
		mat->addTexUniform("diffuseMap", pointerTex, false);

		Quad *quad = new Quad(Vector3(20, 20, 0), false);
		quad->setMaterial(mat);

		cursorNode = new Node();
		cursorNode->attachMesh(quad);
		root->getGuiNode()->attachChild(cursorNode);
	}

	void ActiveGameState::removeDragbox(){
		Root::getSingleton()->getRootNode()->dettachChild(dragboxNode);
		delete dragboxNode;
	}

    void ActiveGameState::onAttached() {
        AbstractAppState::onAttached();
		ConcreteGuiManager::getSingleton()->readLuaScreenScript("activeGameState.lua");

		if(!cursorNode) initCursor();
    }

    void ActiveGameState::onDettached() {
		AbstractAppState::onDettached();
    }

	void ActiveGameState::updateCursor(){
		Vector2 cursorPos = getCursorPos();
		cursorNode->setPosition(Vector3(cursorPos.x, cursorPos.y, .8));

		bool canSelect = canSelectHoveredOnGameObj();
		bool ownGameObj = (gameObjHoveredOn && gameObjHoveredOn->getPlayer()->getTeam() == mainPlayer->getTeam());

		if(controlPressed || (gameObjHoveredOn && !ownGameObj && gameObjHoveredOn->getType() == GameObject::Type::UNIT))
			cursorState = CursorState::ATTACK;
		else
			cursorState = CursorState::NORMAL;

		cursorNode->setVisible(cursorState != CursorState::NORMAL);
		Texture *tex = nullptr;

		switch(cursorState){
			case CursorState::ATTACK:
				tex = attackTex;
				break;
			case CursorState::GARRISON:
				tex = garrisonTex;
				break;
		}

		cursorNode->getMesh(0)->getMaterial()->setTexUniform("diffuseMap", tex, false);
	}

    void ActiveGameState::update() {
		ConcreteGuiManager *guiManager = ConcreteGuiManager::getSingleton();
		guiManager->getText("depth")->setText(L"Depth: " + to_wstring(depth));
		guiManager->getText("refineds")->setText(to_wstring(mainPlayer->getResource(ResourceType::REFINEDS)));
		guiManager->getText("wealth")->setText(to_wstring(mainPlayer->getResource(ResourceType::WEALTH)));
		guiManager->getText("research")->setText(to_wstring(mainPlayer->getResource(ResourceType::RESEARCH)));

		updateCursor();
		Vector2 cursorPos = getCursorPos();

		float eps = .01;

		if(!isSelectionBox && leftMouseClicked && (fabs(clickPoint.x - cursorPos.x) > eps || fabs(clickPoint.y - cursorPos.y) > eps) && getTime() - lastLeftMouseClicked > 10)
			isSelectionBox = true;
		else if (isSelectionBox)
			updateDragBox();

		dragboxNode->setVisible(isSelectionBox);

		updateGameObjHoveredOn();

		vector<Unit*> selectedUnits = mainPlayer->getSelectedUnits();
		GameObjectFrameController *fc = GameObjectFrameController::getSingleton();

		if(!selectingDestOrient && leftMouseClicked && !selectedUnits.empty() && getTime() - lastLeftMouseClicked > 100){
			selectingDestOrient = true;

			fc->setPlacingFrames(true);
			fc->setRotatingFrames(true);

			for(Unit *unit : selectedUnits)
				fc->addGameObjectFrame(GameObjectFrame(unit->getId(), GameObject::Type::UNIT));
		}

        renderUnits();

		if(fc->isPlacingFrames())
			fc->update();

		CameraController *camCtr = CameraController::getSingleton();

		if(!camCtr->isLookingAround())
			camCtr->updateCameraPosition();
    }

	void ActiveGameState::updateGameObjHoveredOn(){
		vector<GameObject*> gameObjs;
		vector<Player*> players = Game::getSingleton()->getPlayers();

		for(Player *pl : players){
			vector<ResourceDeposit*> resDeps = pl->getResourceDeposits();
			vector<Unit*> units = pl->getUnits();

			for(ResourceDeposit *rd : resDeps)
				gameObjs.push_back((GameObject*)rd);

			for(Unit *u : units)
				gameObjs.push_back((GameObject*)u);
		}

		for(GameObject *gameObj : gameObjs)
			if(isGameObjSelectable(gameObj, false)){
				gameObjHoveredOn = gameObj;
				return;
			}

		gameObjHoveredOn = nullptr;
	}

	void ActiveGameState::deselectUnits(){
		mainPlayer->deselectUnits();
		GameObjectFrameController *ufCtr = GameObjectFrameController::getSingleton();
		ufCtr->removeGameObjectFrames();
		ufCtr->setPlacingFrames(false);

		unitGuiScreen = "";
		ConcreteGuiManager::getSingleton()->readLuaScreenScript("activeGameState.lua");
	}

	bool ActiveGameState::selectedUnitsAmongst(vector<Unit*> units){
		vector<Unit*> selUnits = mainPlayer->getSelectedUnits();

		for(Unit *unit : units)
			if(find(selUnits.begin(), selUnits.end(), unit) != selUnits.end())
				return true;

		return false;
	}

	bool ActiveGameState::isGameObjSelectable(GameObject *obj, bool useDragBox){
		if(!obj->getModel()->isVisible()) return false;

		Node *hitboxNode = obj->getHitbox();
		Box *hitbox = (Box*)hitboxNode->getMesh(0);

		Vector3 hitboxSize = hitbox->getSize();
		float width = hitboxSize.x, height = hitboxSize.y, length = hitboxSize.z;
		const int NUM_CORNERS = 8;

		Vector3 corners[NUM_CORNERS]{
			Vector3(-.5 * width, -.5 * height, -.5 * length),
			Vector3(-.5 * width, -.5 * height, .5 * length),
			Vector3(.5 * width, -.5 * height, .5 * length),
			Vector3(.5 * width, -.5 * height, -.5 * length),
			Vector3(-.5 * width, .5 * height, -.5 * length),
			Vector3(-.5 * width, .5 * height, .5 * length),
			Vector3(.5 * width, .5 * height, .5 * length),
			Vector3(.5 * width, .5 * height, -.5 * length),
		};

		Vector2 cornersOnScreen[NUM_CORNERS];
		Vector3 hitboxOffset = hitboxNode->getPosition();

		for(int i = 0; i < NUM_CORNERS; i++){
			Vector3 cornerInWorld = 
				obj->getPos() + 
				obj->getLeftVec() * (corners[i].x + hitboxOffset.x) +
			   	obj->getUpVec() * (corners[i].y + hitboxOffset.y) + 
				obj->getDirVec() * (corners[i].z + hitboxOffset.z);

			Vector3 screenSpace3d = spaceToScreen3d(cornerInWorld);

			if(fabs(screenSpace3d.z) > 1) return false;

			cornersOnScreen[i] = Vector2(screenSpace3d.x, screenSpace3d.y);
		}

		vector<Vector2> selectionPoints;
		Vector3 dragboxOrigin = Vector3::VEC_ZERO, dragboxSize = Vector3::VEC_ZERO;

		if(useDragBox){
			dragboxOrigin = dragboxNode->getPosition(); 
			dragboxSize = ((Quad*)dragboxNode->getMesh(0))->getSize();
			Vector2 objScreenPos = spaceToScreen(obj->getPos()); 

			if(
					(dragboxOrigin.x < objScreenPos.x && objScreenPos.x < dragboxOrigin.x + dragboxSize.x) &&
					(dragboxOrigin.y < objScreenPos.y && objScreenPos.y < dragboxOrigin.y + dragboxSize.y)
			)
				return true;

			selectionPoints = vector<Vector2>{
				Vector2(dragboxOrigin.x, dragboxOrigin.y),
				Vector2(dragboxOrigin.x + dragboxSize.x, dragboxOrigin.y),
				Vector2(dragboxOrigin.x + dragboxSize.x, dragboxOrigin.y + dragboxSize.y),
				Vector2(dragboxOrigin.x, dragboxOrigin.y + dragboxSize.y)
			};
		}
		else
			selectionPoints = vector<Vector2>{getCursorPos()};

		int numAboveEdges = 0, numBelowEdges = 0;

		for(int i = 0; i < selectionPoints.size(); i++){
			const int NUM_EDGES = 12;
			int edges[NUM_EDGES][2]{{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

			for(int j = 0; j < NUM_EDGES && (numAboveEdges == 0 || numBelowEdges == 0); j++){
				Vector2 vertA = cornersOnScreen[edges[j][0]], vertB = cornersOnScreen[edges[j][1]];
				float edgeHorLen = fabs(vertA.x - vertB.x), edgeVertLen = fabs(vertA.y - vertB.y);

				if(fabs(selectionPoints[i].x - vertA.x) < edgeHorLen && fabs(selectionPoints[i].x - vertB.x) < edgeHorLen){
					if(vertA.x > selectionPoints[i].x) swap(vertA, vertB); 

					float horOffset = (selectionPoints[i].x - vertA.x) / edgeHorLen;
					float height = vertA.y + (vertA.y < vertB.y ? 1 : -1) * horOffset * edgeVertLen;

					(selectionPoints[i].y > height ? numAboveEdges : numBelowEdges)++;
				}
			}
		}

		return (numAboveEdges > 0 && numBelowEdges > 0);
	}

	//TODO fix fog of war for hostile units
    void ActiveGameState::renderUnits() {
		ConcreteGuiManager *guiManager = ConcreteGuiManager::getSingleton();
		vector<Listbox*> listboxes{};
		vector<Checkbox*> checkboxes{};
		vector<Slider*> sliders{};
		vector<Textbox*> textboxes{};
		vector<Node*> guiRects = guiManager->getGuiRectangles();
		vector<Text*> texts{
			guiManager->getText("depth"),
			guiManager->getText("refineds"),
			guiManager->getText("wealth"),
			guiManager->getText("research")
		};

		vector<Unit*> units;

        for (Player *p : Game::getSingleton()->getPlayers())
            for (Unit *u : p->getUnits())
                units.push_back(u);

		vector<Unit*> selUnits = mainPlayer->getSelectedUnits();

        for (Unit *u : units) {
			Node *model = u->getModel();

            if (u->getPlayer() == mainPlayer){
				if(!u->getLosLightNode()) u->initLosLight();

				model->setVisible(true);

            	Vector2 pos = u->getScreenPos();
				string guiScreen = u->getGuiScreen();

				if(!selUnits.empty() && u == selUnits[0] && guiScreen != "" && guiScreen != unitGuiScreen){
					guiManager->readLuaScreenScript(u->getGuiScreen(), buttons, listboxes, checkboxes, sliders, textboxes, guiRects, texts);
					unitGuiScreen = guiScreen;
				}
            }
			else{
				if(u->getLosLightNode()) u->destroyLosLight();

				model->setVisible(false);

            	for (int i = 0; i < units.size(); i++)
					if(units[i]->getPlayer() == mainPlayer){
            			Vector3 obsUnitPos = units[i]->getPos();
            			obsUnitPos.y = 0;

						Vector3 compUnitPos = u->getPos();
						compUnitPos.y = 0;
						float dist = compUnitPos.getDistanceFrom(obsUnitPos);

						if(dist <= units[i]->getLineOfSight()){
							model->setVisible(true);
							break;
						}
					}
			}
        }
    }

	//TODO improve this for greater accuracy
    bool ActiveGameState::isInLineOfSight(Vector3 center, float radius, Unit *u) {
        bool inside = false;

        for (int i = 0; i < 4 && !inside; i++)
            if (center.getDistanceFrom(u->getCorner(i)) <= radius){
                inside = true;
				break;
			}

        return inside;
    }

    void ActiveGameState::updateDragBox() {
        Vector2 mousePos = getCursorPos(), dragBoxOrigin, dragBoxEnd;

        if (mousePos.x >= clickPoint.x && mousePos.y >= clickPoint.y) {
            dragBoxOrigin = Vector2(clickPoint.x, clickPoint.y);
            dragBoxEnd = Vector2(mousePos.x, mousePos.y);
        }
		else if (mousePos.x < clickPoint.x && mousePos.y > clickPoint.y) {
            dragBoxOrigin = Vector2(mousePos.x, clickPoint.y);
            dragBoxEnd = Vector2(clickPoint.x, mousePos.y);
        }
		else if (mousePos.x >= clickPoint.x && mousePos.y < clickPoint.y) {
            dragBoxOrigin = Vector2(clickPoint.x, mousePos.y);
            dragBoxEnd = Vector2(mousePos.x, clickPoint.y);
        }
		else {
            dragBoxOrigin = Vector2(mousePos.x, mousePos.y);
            dragBoxEnd = Vector2(clickPoint.x, clickPoint.y);
        }

		Vector3 size = Vector3(dragBoxEnd.x - dragBoxOrigin.x, dragBoxEnd.y - dragBoxOrigin.y, 0);
		Quad *dragbox = (Quad*)dragboxNode->getMesh(0);
		dragbox->setSize(size);
		dragbox->updateVerts(dragbox->getMeshBase());
		dragboxNode->setPosition(Vector3(dragBoxOrigin.x, dragBoxOrigin.y, 0));
    }

	//TODO fix ejectable unit selection with multiple transports selected
    void ActiveGameState::issueOrder(Order::TYPE type, vector<Order::Target> targets, bool addOrder) {
		Vector3 destDir = (selectingDestOrient ? GameObjectFrameController::getSingleton()->getGameObjectFrame(0).getDirVec() : Vector3::VEC_ZERO);
		mainPlayer->issueOrder(type, destDir, targets, addOrder);
		this->targets.clear();
    }
    
    void ActiveGameState::castRayToTerrain() {
		Camera *cam = Root::getSingleton()->getCamera();
		Vector3 camPos = cam->getPosition();
		Vector3 endPos = screenToSpace(getCursorPos());

		Vector3 rayDir = (endPos - camPos).norm();
		Map *map = Map::getSingleton();
		vector<RayCaster::CollisionResult> results = map->raycastTerrain(camPos, rayDir, true);

		if(!results.empty()){
			GameObjectFrameController *ufCtr = GameObjectFrameController::getSingleton();

			for(Unit *u : mainPlayer->getSelectedUnits()){
				Vector3 pos = u->getPos();
				Node *nodeParent = map->getNodeParent();

				if(u->getType() == UnitType::UNDERWATER && nodeParent->getNumChildren() > 0){
					Vector3 cellSize = map->getCellSize(), waterBodyPos;
					bool inWater = false;

					for(int i = 1; i < nodeParent->getNumChildren(); i++){
						Vector3 wPos = nodeParent->getChild(i)->getPosition();
						Vector3 wSize = ((Quad*)nodeParent->getChild(i)->getMesh(0))->getSize();
						
						if(fabs(wPos.x - pos.x) < .5 * wSize.x && fabs(wPos.z - pos.z) < .5 * wSize.y){
							waterBodyPos = wPos;
							inWater = true;
							break;
						}
					}

					if(inWater){
						vector<RayCaster::CollisionResult> res = map->raycastTerrain(
								Vector3(results[0].pos.x, 100, results[0].pos.z), 
								-Vector3::VEC_J,
								false
						);

						vector<Map::Cell> &cells = map->getCells();
						int cid = map->getCellId(results[0].pos, false);
						int numSubmarineCells = cells[cid].underWaterCellIds.size();
						float maxDepth = cells[cid].pos.y;
						float minDepth = (numSubmarineCells > 0 ? cells[cells[cid].underWaterCellIds[numSubmarineCells - 1]].pos.y : maxDepth) - .5 * cellSize.y;

						float newDepth = res[0].pos.y + depth * (waterBodyPos.y - res[0].pos.y);

						if(newDepth < minDepth) newDepth = minDepth;
						else if(newDepth > maxDepth) newDepth = maxDepth;

						results[0].pos.y = newDepth;
					}
				}
			}

			targets.push_back(Order::Target(nullptr, results[0].pos));
		}
    }

	void ActiveGameState::enableUnitState(Unit::State state){
		vector<Unit*> selectedUnits = mainPlayer->getSelectedUnits();

		for(Unit *unit : selectedUnits)
			unit->setState(state);
	}
    
	//TODO only allow appropriate orders for units
    void ActiveGameState::onAction(int bind, bool isPressed) {
		GameObjectFrameController *ufCtr = GameObjectFrameController::getSingleton();

		if(!ConcreteGuiManager::getSingleton()->findClickedButtons().empty())
			return;

        switch((Bind)bind){
			case Bind::DRAG_BOX: 
				leftMouseClicked = isPressed;

				if(isPressed){
					clickPoint = getCursorPos();
					lastLeftMouseClicked = getTime();
				}
				else{
					int numSelectedUnits = mainPlayer->getNumSelectedUnits();

                    if(numSelectedUnits > 0){
						if(selectingPatrolPoints){
                    		castRayToTerrain();

							if(!(targets.empty() || selectingPatrolPoints)){
								Order::TYPE type = Order::TYPE::MOVE;
							
								if(controlPressed || (targets[0].unit && targets[0].unit->getPlayer()->getTeam() != mainPlayer->getTeam()))
									type = Order::TYPE::ATTACK;
								else if(ufCtr->isPlacingFrames() && !selectingDestOrient)
									type = Order::TYPE::BUILD;
							
								issueOrder(type, targets, shiftPressed);
							}
						}
						else if(!(selectingDestOrient || isSelectionBox)){
							bool canSelect = canSelectHoveredOnGameObj();
							bool ownGameObj = (gameObjHoveredOn && gameObjHoveredOn->getPlayer()->getTeam() == mainPlayer->getTeam());

							if(gameObjHoveredOn && gameObjHoveredOn->getType() == GameObject::Type::UNIT && ownGameObj && ((Unit*)gameObjHoveredOn)->getNumGarrisonSlots() > 0){
								bool canGarrison = true;

								for(int i = 0; i < mainPlayer->getNumSelectedUnits(); i++)
									if(!((Unit*)gameObjHoveredOn)->canGarrison((Vehicle*)mainPlayer->getSelectedUnit(i))){
										canGarrison = false;
										break;
									}

								if(canGarrison)
									issueOrder(Order::TYPE::GARRISON, vector<Order::Target>{Order::Target((Unit*)gameObjHoveredOn, gameObjHoveredOn->getPos())}, shiftPressed);
							}
							else if(gameObjHoveredOn && ownGameObj && ((Unit*)gameObjHoveredOn)->getUnitClass() == UnitClass::EXTRACTOR && !controlPressed){
								issueOrder(Order::TYPE::SUPPLY, vector<Order::Target>{Order::Target((Unit*)gameObjHoveredOn)}, shiftPressed);
							}
							else if(gameObjHoveredOn && (controlPressed || (!ownGameObj && gameObjHoveredOn->getType() == GameObject::Type::UNIT)))
								issueOrder(Order::TYPE::ATTACK, vector<Order::Target>{Order::Target((Unit*)gameObjHoveredOn, gameObjHoveredOn->getPos())}, shiftPressed);
							else if(controlPressed && !gameObjHoveredOn){
                    			castRayToTerrain();
								issueOrder(Order::TYPE::ATTACK, targets, shiftPressed);
							}
							else if(ufCtr->isPlacingFrames() && !selectingDestOrient){
                    			castRayToTerrain();
								GameObjectFrame gmObjFr = ufCtr->getGameObjectFrame(0);

								if(gmObjFr.status == GameObjectFrame::NOT_PLACEABLE)
									return;

								Unit *buildStruct = GameObjectFactory::createUnit(mainPlayer, gmObjFr.getId(), gmObjFr.getPos(), gmObjFr.getRot());
								mainPlayer->addUnit(buildStruct);
								targets[0].unit = buildStruct;

								issueOrder(Order::TYPE::BUILD, targets, shiftPressed);
							}
							else if(canSelect){
								if(!shiftPressed)
									deselectUnits();

								mainPlayer->selectUnit((Unit*)gameObjHoveredOn);
							}
							else if(!canSelect){
                    			castRayToTerrain();
								issueOrder(Order::TYPE::MOVE, targets, shiftPressed);
							}
						}
					}
					else if(numSelectedUnits == 0 && canSelectHoveredOnGameObj()){
						if(!shiftPressed) deselectUnits();

						mainPlayer->selectUnit((Unit*)gameObjHoveredOn);
					}

					selectingDestOrient = false;

					if(isSelectionBox){
						if(!shiftPressed) deselectUnits();

						vector<Unit*> units = mainPlayer->getUnits();

						for(Unit *un : units)
							if(isGameObjSelectable(un, true))
								mainPlayer->selectUnit(un);
					}

					isSelectionBox = false;
					Quad *quad = (Quad*)dragboxNode->getMesh(0);
					quad->setSize(Vector3::VEC_ZERO);
					quad->updateVerts(quad->getMeshBase());

					ufCtr->setPlacingFrames(false);
					ufCtr->setRotatingFrames(false);
					ufCtr->removeGameObjectFrames();
                }

                break;
			case Bind::ENABLE_CHASE_STATE:
				if(isPressed) enableUnitState(Unit::State::CHASE);
				break;
			case Bind::ENABLE_STAND_GROUND_STATE:
				if(isPressed) enableUnitState(Unit::State::STAND_GROUND);
				break;
			case Bind::ENABLE_HOLD_FIRE_STATE:
				if(isPressed) enableUnitState(Unit::State::HOLD_FIRE);
				break;
			case Bind::DESELECT:
				if(ufCtr->isPlacingFrames()) ufCtr->setRotatingFrames(isPressed);
                if (!isPressed) deselectUnits();
                break;
			case Bind::EJECT_GARRISON:
				if(isPressed) issueOrder(Order::TYPE::EJECT, vector<Order::Target>{}, shiftPressed);
				break;
			case Bind::LAUNCH:
				if(isPressed){
					castRayToTerrain();
					issueOrder(Order::TYPE::LAUNCH, targets, shiftPressed);
				}

				break;
			case Bind::HACK:
				if(isPressed){
					if(gameObjHoveredOn && gameObjHoveredOn->getType() == GameObject::Type::UNIT && gameObjHoveredOn->getPlayer()->getTeam() != mainPlayer->getTeam())
						issueOrder(Order::TYPE::HACK, vector<Order::Target>{Order::Target((Unit*)gameObjHoveredOn)}, shiftPressed);

					break;
				}
			case Bind::TOGGLE_SUB:
                break;
			case Bind::ZOOM_IN:
                if (zooms > -NUM_MAX_ZOOMS) {
					Camera *cam = Root::getSingleton()->getCamera();
                    cam->setPosition(cam->getPosition() + cam->getDirection().norm() * .5f);
                    zooms--;
                }
                break;
			case Bind::ZOOM_OUT:
                if (zooms < NUM_MAX_ZOOMS) {
					Camera *cam = Root::getSingleton()->getCamera();
                    cam->setPosition(cam->getPosition() - (cam->getDirection().norm().norm() * .5f));
                    zooms++;
                }
                break;
			case Bind::LOOK_AROUND:
				if(!ufCtr->isPlacingFrames())
					CameraController::getSingleton()->setLookingAround(isPressed);
                break;
			case Bind::HALT: 
				if(isPressed)
            		for (Unit *u : mainPlayer->getSelectedUnits())
                    	u->halt();
                break;
				//TODO reimplement submarine diving mechanic
			case Bind::LEFT_CONTROL:
                controlPressed = isPressed;
				if(isPressed) depth -= 0.05;
                break;
			case Bind::LEFT_SHIFT:
			{
        		shiftPressed = isPressed;

				GameObjectFrameController *ufCtr = GameObjectFrameController::getSingleton();
				ufCtr->setPaintSelecting(shiftPressed);

				if(isPressed){
					depth += 0.05;

					Vector3 startPos = Root::getSingleton()->getCamera()->getPosition();
					vector<RayCaster::CollisionResult> results = Map::getSingleton()->raycastTerrain(startPos, (screenToSpace(getCursorPos()) - startPos).norm(), true);

					if(!results.empty())
						ufCtr->setPaintSelectRowStart(results[0].pos);
				}
			}
                break;
			case Bind::SELECT_PATROL_POINTS: 
				if(isPressed && mainPlayer->getNumSelectedUnits() > 0){
					targets.push_back(Order::Target(nullptr, mainPlayer->getSelectedUnit(0)->getPos()));
			   		selectingPatrolPoints = isPressed;
				}

                break;
			case Bind::GROUP_0:
            case Bind::GROUP_1:
            case Bind::GROUP_2:
            case Bind::GROUP_3:
            case Bind::GROUP_4:
            case Bind::GROUP_5:
            case Bind::GROUP_6:
            case Bind::GROUP_7:
            case Bind::GROUP_8:
            case Bind::GROUP_9:
                if(isPressed){
                    int group = bind - Bind::GROUP_0;

                    if(controlPressed)
                        unitGroups[group] = mainPlayer->getSelectedUnits();
                    else{
                        if(!shiftPressed)
                            mainPlayer->selectUnits(unitGroups[group]);
                        else
                            for(Unit *u : unitGroups[group])
                                mainPlayer->selectUnit(u);
                    }
                }

                break;
			case Bind::DESELECT_STRUCTURE:
				ufCtr->removeGameObjectFrames();
				ufCtr->setPlacingFrames(false);
				break;
        }
    }

    void ActiveGameState::onAnalog(int bind, float strength) {
		CameraController *camCtr = CameraController::getSingleton();
		GameObjectFrameController *ufCtr = GameObjectFrameController::getSingleton();

		switch((Bind)bind){
			case Bind::LOOK_UP: 
			case Bind::LOOK_DOWN: 
				if(camCtr->isLookingAround()) {
					Vector3 dirProj = Root::getSingleton()->getCamera()->getDirection();
					dirProj = Vector3(dirProj.x, 0, dirProj.z).norm();
					CameraController::getSingleton()->orientCamera(Vector3(0, 1, 0).cross(dirProj), strength);
				}

				break;
			case Bind::LOOK_LEFT: 
			case Bind::LOOK_RIGHT: 
				if(camCtr->isLookingAround())
					CameraController::getSingleton()->orientCamera(Vector3(0, 1, 0), strength);
				else if(ufCtr->isRotatingFrames())
					ufCtr->rotateGameObjectFrames(100 * strength);

				break;
		}
	}

	void ActiveGameState::onRawMouseWheelScroll(bool up){
		CameraController::getSingleton()->zoomCamera(up);
	}
}
