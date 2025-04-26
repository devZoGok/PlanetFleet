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
#include "unitButton.h"
#include "extractor.h"
#include "resourceDeposit.h"
#include "resourceRover.h"
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
			   st = SOL_LUA_VIEW["supplyTex"],
			   ht = SOL_LUA_VIEW["hackTex"],
			   ngt = SOL_LUA_VIEW["noGarrisonTex"],
			   nst = SOL_LUA_VIEW["noSupplyTex"],
			   nht = SOL_LUA_VIEW["noHackTex"],
			   p1[]{basePath + pt}, 
			   p2[]{basePath + at}, 
			   p3[]{basePath + gt}, 
			   p4[]{basePath + st},
			   p5[]{basePath + ht},
			   p6[]{basePath + ngt}, 
			   p7[]{basePath + nst},
			   p8[]{basePath + nht};
		pointerTex = new Texture(p1, 1, false);
		attackTex = new Texture(p2, 1, false);
		garrisonTex = new Texture(p3, 1, false);
		supplyTex = new Texture(p4, 1, false);
		hackTex = new Texture(p5, 1, false);
		noGarrisonTex = new Texture(p6, 1, false);
		noSupplyTex = new Texture(p7, 1, false);
		noHackTex = new Texture(p8, 1, false);

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

		if(mainPlayer->getNumSelectedUnits() > 0){
			bool ownGameObj = (gameObjHoveredOn && gameObjHoveredOn->getPlayer() == mainPlayer);
			bool alliedGameObj = (gameObjHoveredOn && !ownGameObj && gameObjHoveredOn->getPlayer()->getTeam() == mainPlayer->getTeam());
			bool gameObjUnit = (gameObjHoveredOn && gameObjHoveredOn->getType() == GameObject::Type::UNIT);
			bool gameObjTransport = (gameObjUnit && ((Unit*)gameObjHoveredOn)->getNumGarrisonSlots() > 0);
			bool canGarrison = true;

			if(gameObjUnit && gameObjTransport)
				for(int i = 0; i < mainPlayer->getNumSelectedUnits(); i++)
					if(!((Unit*)gameObjHoveredOn)->canGarrison((Vehicle*)mainPlayer->getSelectedUnit(i))){
						canGarrison = false;
						break;
					}

			vector<TradeOffer*> offers = (gameObjHoveredOn && alliedGameObj ? mainPlayer->getTradeOffers(gameObjHoveredOn->getPlayer()) : vector<TradeOffer*>{});
			bool tradeResource[NUM_RESOURCES][2], trade = false;

			for(int i = 0; i < NUM_RESOURCES; i++)
				for(int j = 0; j < 2; j++){
					if(!offers.empty() && offers[0]->tradeResources[i][j] > 0){
						tradeResource[i][j] = true;
						trade = true;
					}
					else tradeResource[i][j] = false;
				}

			int objClass = (gameObjUnit ? int(((Unit*)gameObjHoveredOn)->getUnitClass()) : -1);
			bool tradeCenter = ((UnitClass)objClass == UnitClass::TRADE_CENTER);
			bool refinery = ((UnitClass)objClass == UnitClass::REFINERY);
			bool lab = ((UnitClass)objClass == UnitClass::LAB);
			bool ownExtractor = (ownGameObj && (UnitClass)objClass == UnitClass::EXTRACTOR);

			bool roverSelected = (mainPlayer->getSelectedUnit(0)->getUnitClass() == UnitClass::RESOURCE_ROVER);
			ResourceRover *rover = (roverSelected ? (ResourceRover*)mainPlayer->getSelectedUnit(0) : nullptr);
			int unloadFlag = (int)shiftPressed;

			if(!forceCursorState){
				if(gameObjUnit && gameObjTransport){
					orderPossible = (ownGameObj && canGarrison);
					cursorState = CursorState::GARRISON;
				}
				else if(roverSelected && ownExtractor){
					orderPossible = (((Extractor*)gameObjHoveredOn)->getDeposit()->getAmmount() > 0);
					cursorState = CursorState::SUPPLY;
				}
				else if(roverSelected && (tradeCenter || refinery || lab) && (ownGameObj || (alliedGameObj && trade))){
					bool tradeRes = false;
					int resId;
					
					switch((UnitClass)objClass){
						case UnitClass::TRADE_CENTER:
							tradeRes = tradeResource[(int)ResourceType::WEALTH][unloadFlag];
							resId = (int)ResourceType::WEALTH;
							break;
						case UnitClass::REFINERY:
							tradeRes = tradeResource[(int)ResourceType::REFINEDS][unloadFlag];
							resId = (int)ResourceType::REFINEDS;
							break;
						case UnitClass::LAB:
							tradeRes = tradeResource[(int)ResourceType::RESEARCH][unloadFlag];
							resId = (int)ResourceType::RESEARCH;
							break;
					}
					
					bool canLoad = (rover && rover->calcTotalLoad() < rover->getCapacity());
					orderPossible = (unloadFlag ? rover->getLoad(resId) > 0 : canLoad);

					if(alliedGameObj) orderPossible = tradeRes && orderPossible;

					cursorState = (unloadFlag ? CursorState::UNLOAD : CursorState::LOAD);
				}
				else if(gameObjUnit && !(ownGameObj || alliedGameObj))
					cursorState = CursorState::ATTACK;
				else
					cursorState = CursorState::NORMAL;
			}
			else{
				if(cursorState == CursorState::GARRISON)
					orderPossible = (ownGameObj && gameObjTransport && canGarrison);
				else if(cursorState == CursorState::SUPPLY)
					orderPossible = (roverSelected && ownExtractor);
				else if(cursorState == CursorState::LOAD || cursorState == CursorState::UNLOAD){
					int ulf = int(cursorState == CursorState::UNLOAD);
					bool tradeRefs = tradeResource[(int)ResourceType::REFINEDS][ulf];
					bool tradeWealth = tradeResource[(int)ResourceType::WEALTH][ulf];
					bool tradeTech = tradeResource[(int)ResourceType::RESEARCH][ulf];
					
					bool canLoad = (rover->calcTotalLoad() < rover->getCapacity());
					bool loadResource = (
							cursorState == CursorState::LOAD ? canLoad : (
								rover->getLoad((int)ResourceType::REFINEDS) > 0 ||
								rover->getLoad((int)ResourceType::WEALTH) > 0 ||
								rover->getLoad((int)ResourceType::RESEARCH) > 0
							)
					);

					bool transferResource = (
						(refinery && loadResource && (ownGameObj || (alliedGameObj && tradeRefs))) ||
						(tradeCenter && loadResource && (ownGameObj || (alliedGameObj && tradeWealth))) ||
						(lab && loadResource && (ownGameObj || (alliedGameObj && tradeTech)))
					);
					orderPossible = (roverSelected && transferResource);
				}
				else if(cursorState == CursorState::HACK)
					orderPossible = (!ownGameObj && gameObjUnit && mainPlayer->getSelectedUnit(0)->getUnitClass() == UnitClass::ENGINEER);
				else if(gameObjUnit)
					cursorState = CursorState::ATTACK;
				else
					cursorState = CursorState::NORMAL;
			}
		}

		cursorNode->setVisible(cursorState != CursorState::NORMAL);
		Texture *tex = nullptr;

		switch(cursorState){
			case CursorState::ATTACK:
				tex = attackTex;
				break;
			case CursorState::GARRISON:
				tex = (orderPossible ? garrisonTex : noGarrisonTex);
				break;
			case CursorState::SUPPLY:
			case CursorState::LOAD:
			case CursorState::UNLOAD:
				tex = (orderPossible ? supplyTex : noSupplyTex);
				break;
			case CursorState::HACK:
				tex = (orderPossible ? hackTex : noHackTex);
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

		if(!isSelectionBox && selectMouseClicked && (fabs(clickPoint.x - cursorPos.x) > eps || fabs(clickPoint.y - cursorPos.y) > eps) && getTime() - lastSelectMouseClicked > 10)
			isSelectionBox = true;
		else if (isSelectionBox)
			updateDragBox();

		dragboxNode->setVisible(isSelectionBox);

		updateGameObjHoveredOn();

		vector<Unit*> selectedUnits = mainPlayer->getSelectedUnits();
		GameObjectFrameController *fc = GameObjectFrameController::getSingleton();

		if(!selectingDestOrient && orderMouseClicked && !selectedUnits.empty() && getTime() - lastOrderMouseClicked > 100){
			if(targets.empty()) castRayToTerrain();

			if(!buildableStructSelected)
				for(int i = 0; i < selectedUnits.size(); i++){
					fc->addGameObjectFrame(GameObjectFrame(selectedUnits[i]->getId(), GameObject::Type::UNIT));
					fc->placeGameObjectFrame(fc->getNumGameObjectFrames() - 1, targets[0].pos, selectedUnits[i]->getWidth(), selectedUnits[i]->getLength());
				}

			selectingDestOrient = true;
			fc->setRotating(true);
		}
		else if(selectingDestOrient && !orderMouseClicked){
			selectingDestOrient = false;

			fc->toggleFrameTransformations(false, false, false);
			fc->removeGameObjectFrames();
		}

		if(!tradingScreen){
			guiTexts = guiManager->getTexts();
			guiRects = guiManager->getGuiRectangles();
			guiButtons = guiManager->getButtons();
		}

        renderUnits();

		vector<Unit*> currSelectedUnits = mainPlayer->getSelectedUnits();

		if(prevSelectedUnits != currSelectedUnits){
			addUnitGui();
			prevSelectedUnits = currSelectedUnits;
		}

		if(fc->isRotating() || fc->isPlacingOnSurface() || fc->isPlacingVertically())
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
		ufCtr->toggleFrameTransformations(false, false, false);

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
	//TODO replace shiftPressed with a dedicated buy flag
    void ActiveGameState::issueOrder(Order::TYPE type, vector<Order::Target> targets, bool addOrder) {
		depth = 1;
		Vector3 destDir =  Vector3::VEC_ZERO;
		
		if(selectingDestOrient){
			GameObjectFrame &objFrame = GameObjectFrameController::getSingleton()->getGameObjectFrame(0);
			destDir = objFrame.getDirVec();
			targets[0].pos = objFrame.getPos();
		}

		mainPlayer->issueOrder(type, destDir, targets, addOrder);
		forceCursorState = false;
		this->targets.clear();
    }
    
    void ActiveGameState::castRayToTerrain() {
		Camera *cam = Root::getSingleton()->getCamera();
		Vector3 camPos = cam->getPosition();
		Vector3 endPos = screenToSpace(getCursorPos());

		Vector3 rayDir = (endPos - camPos).norm();
		Map *map = Map::getSingleton();
		vector<RayCaster::CollisionResult> results = map->raycastTerrain(camPos, rayDir, true);

		if(!results.empty())
			targets.push_back(Order::Target(nullptr, results[0].pos));
    }

	void ActiveGameState::enableUnitState(Unit::State state){
		vector<Unit*> selectedUnits = mainPlayer->getSelectedUnits();

		for(Unit *unit : selectedUnits)
			unit->setState(state);
	}

	void ActiveGameState::addUnitGui(){
		vector<Unit*> currSelectedUnits = mainPlayer->getSelectedUnits();

		if(currSelectedUnits.empty()) return;

		ConcreteGuiManager *guiManager = ConcreteGuiManager::getSingleton();

		sol::state_view SOL_LUA_VIEW = generateView();
		SOL_LUA_VIEW.script("_mainUnitId = " + to_string(currSelectedUnits[0]->getId()));
		guiManager->readLuaScreenScriptDel("unitGui.lua", unitButtons);

		SOL_LUA_VIEW.script("numGui = #gui");
		int numButtons = SOL_LUA_VIEW["numGui"];

		for(int i = 0; i < numButtons; i++)
			unitButtons.push_back(guiManager->getButtons()[guiManager->getButtons().size() - (i + 1)]);
	}
    
    void ActiveGameState::onAction(int bind, bool isPressed) {
		if(tradingScreen || !ConcreteGuiManager::getSingleton()->findClickedButtons().empty()) return;

		GameObjectFrameController *ufCtr = GameObjectFrameController::getSingleton();
		vector<Unit*> selectedUnits = mainPlayer->getSelectedUnits();

        switch((Bind)bind){
			case Bind::DRAG_BOX: 
				selectMouseClicked = isPressed;

				if(isPressed){
					clickPoint = getCursorPos();
					lastSelectMouseClicked = getTime();
				}
				else{
					if(!shiftPressed) deselectUnits();

					if(canSelectHoveredOnGameObj())
						mainPlayer->selectUnit((Unit*)gameObjHoveredOn);

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

					ufCtr->toggleFrameTransformations(false, false, false);
					ufCtr->removeGameObjectFrames();
                }

                break;
			case Bind::ROTATE_OBJ_FRAME:
				orderMouseClicked = isPressed;

				if(isPressed)
					lastOrderMouseClicked = getTime();
				else if(!(isPressed || selectedUnits.empty())){
					if(selectingPatrolPoints){
                    	castRayToTerrain();

						if(!(targets.empty() || selectingPatrolPoints)){
							Order::TYPE type = Order::TYPE::MOVE;
						
							if(controlPressed || (targets[0].unit && targets[0].unit->getPlayer()->getTeam() != mainPlayer->getTeam()))
								type = Order::TYPE::ATTACK;
							else if(ufCtr->isPlacingOnSurface() && !selectingDestOrient)
								type = Order::TYPE::BUILD;
						
							issueOrder(type, targets, shiftPressed);
						}
					}
					else if(!isSelectionBox){
						bool ownGameObj = (gameObjHoveredOn && gameObjHoveredOn->getPlayer()->getTeam() == mainPlayer->getTeam());

						if(cursorState == CursorState::GARRISON){
							if(orderPossible) issueOrder(Order::TYPE::GARRISON, vector<Order::Target>{Order::Target((Unit*)gameObjHoveredOn, gameObjHoveredOn->getPos())}, shiftPressed);
						}
						else if(cursorState == CursorState::SUPPLY){
							if(orderPossible) issueOrder(Order::TYPE::SUPPLY, vector<Order::Target>{Order::Target((Unit*)gameObjHoveredOn)}, shiftPressed);
						}
						else if(cursorState == CursorState::LOAD){
							if(orderPossible) issueOrder(Order::TYPE::LOAD, vector<Order::Target>{Order::Target((Unit*)gameObjHoveredOn)}, shiftPressed);
						}
						else if(cursorState == CursorState::UNLOAD){
							if(orderPossible) issueOrder(Order::TYPE::UNLOAD, vector<Order::Target>{Order::Target((Unit*)gameObjHoveredOn)}, shiftPressed);
						}
						else if(cursorState == CursorState::ATTACK){
							if(controlPressed && !gameObjHoveredOn){
                    			castRayToTerrain();
								issueOrder(Order::TYPE::ATTACK, targets, shiftPressed);
							}
							else{
								if(orderPossible) issueOrder(Order::TYPE::ATTACK, vector<Order::Target>{Order::Target((Unit*)gameObjHoveredOn, gameObjHoveredOn->getPos())}, shiftPressed);
							}
						}
						else if(cursorState == CursorState::HACK){
							if(orderPossible) issueOrder(Order::TYPE::HACK, vector<Order::Target>{Order::Target((Unit*)gameObjHoveredOn)}, shiftPressed);
						}
						else if(selectedUnits[0]->getUnitClass() == UnitClass::ENGINEER && ufCtr->isPlacingOnSurface()){
                    		castRayToTerrain();
							GameObjectFrame gmObjFr = ufCtr->getGameObjectFrame(0);

							if(gmObjFr.status == GameObjectFrame::NOT_PLACEABLE) return;

							Unit *buildStruct = GameObjectFactory::createUnit(mainPlayer, gmObjFr.getId(), gmObjFr.getPos(), gmObjFr.getRot());
							mainPlayer->addUnit(buildStruct);
							targets[0].unit = buildStruct;

							issueOrder(Order::TYPE::BUILD, targets, shiftPressed);

							buildableStructSelected = false;
						}
						else if(!canSelectHoveredOnGameObj()){
                    		if(targets.empty()) castRayToTerrain();

							issueOrder(Order::TYPE::MOVE, targets, shiftPressed);
						}
					}
				}

                break;
			case Bind::MOVE_CAMERA:
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
			case Bind::EJECT_GARRISON:
				if(isPressed) issueOrder(Order::TYPE::EJECT, vector<Order::Target>{}, shiftPressed);
				break;
			case Bind::LAUNCH:
				if(isPressed){
					castRayToTerrain();
					issueOrder(Order::TYPE::LAUNCH, targets, shiftPressed);
				}

				break;
				/*
			case Bind::HACK:
				if(isPressed){
					if(gameObjHoveredOn && gameObjHoveredOn->getType() == GameObject::Type::UNIT && gameObjHoveredOn->getPlayer()->getTeam() != mainPlayer->getTeam())
						issueOrder(Order::TYPE::HACK, vector<Order::Target>{Order::Target((Unit*)gameObjHoveredOn)}, shiftPressed);

					break;
				}
				*/
			case Bind::SHIFT_SUB_DEPTH:
				ufCtr->toggleFrameTransformations(false, false, isPressed);
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
                    cam->setPosition(cam->getPosition() - (cam->getDirection().norm() * .5f));
                    zooms++;
                }
                break;
			case Bind::LOOK_AROUND:
				if(!ufCtr->isPlacingOnSurface())
					CameraController::getSingleton()->setLookingAround(isPressed);
                break;
			case Bind::HALT: 
				if(isPressed)
            		for (Unit *u : mainPlayer->getSelectedUnits())
                    	u->halt();
                break;
				//TODO reimplement submarine diving mechanic
			case Bind::LEFT_CONTROL:
				if(isPressed) cursorState = CursorState::ATTACK;

				forceCursorState = isPressed;
				controlPressed = isPressed;
                break;
			case Bind::LEFT_SHIFT:
			{
        		shiftPressed = isPressed;

				ufCtr->setPaintSelecting(shiftPressed);

				if(isPressed){
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
				forceCursorState = false;
				buildableStructSelected = false;
				ufCtr->toggleFrameTransformations(false, false, false);
				ufCtr->removeGameObjectFrames();
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
				else if(ufCtr->isPlacingVertically()){
					depth -= .2 * strength;

					if(depth < 0) depth = 0;
					else if(depth > 1) depth = 1;
				}

				break;
			case Bind::LOOK_LEFT: 
			case Bind::LOOK_RIGHT: 
				if(camCtr->isLookingAround())
					CameraController::getSingleton()->orientCamera(Vector3(0, 1, 0), strength);
				else if(ufCtr->isRotating())
					ufCtr->rotateGameObjectFrames(100 * strength);

				break;
		}
	}

	void ActiveGameState::onRawMouseWheelScroll(bool up){
		CameraController::getSingleton()->zoomCamera(up);
	}
}
