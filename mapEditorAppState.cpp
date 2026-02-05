#include "mapEditorAppState.h"
#include "gameManager.h"
#include "defConfigs.h"
#include "cameraController.h"
#include "resourceDeposit.h"
#include "gameObjectFactory.h"
#include "gameObjectFrameController.h"
#include "player.h"
#include "game.h"
#include "map.h"
#include "util.h"

#include <box.h>
#include <text.h>
#include <node.h>
#include <quad.h>
#include <light.h>
#include <model.h>
#include <texture.h>
#include <assetManager.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/vb01/external/stb/stb_image_write.h"

#include <util.h>

#include <listbox.h>

#include <map>
#include <filesystem>
#include <filesystem>

#include <tinyxml2.h>

namespace battleship{
	using namespace tinyxml2;
	using namespace configData;
	using namespace gameBase;
	using namespace vb01;
	using namespace vb01Gui;
	using namespace std;
	using namespace std::filesystem;

	MapEditorAppState::MapEditor::MapEditor(string name, Vector2 size, bool newMap){
		this->newMap = newMap;

		string basePath = GameManager::getSingleton()->getPath();
		prepareTextures(basePath + "Textures/Skyboxes/", true, skyTextures);
		prepareTextures(basePath + "Textures/Landmass/", false, landmassTextures);
		prepareTextures(basePath + "Textures/Water/", false, waterTextures);

		AssetManager *assetManager = AssetManager::getSingleton();
		assetManager->load(basePath + "Models/Units/", true);
		assetManager->load(basePath + "Models/Resources/", true);
		assetManager->load(basePath + DEFAULT_TEXTURE);

		map = Map::getSingleton();
		Game *game = Game::getSingleton();

		if(newMap){
			game->addPlayer(new Player(0, 0, 0, Vector3(1, 1, 1)));
			map->create(name, Vector3(size.x, 0, size.y));
			generatePlane(size);
		}
		else{
			map->load(name);

			int numPlayers = map->getNumSpawnPoints();
			int numVerts = 3 * map->getNodeParent()->getChild(0)->getMesh(0)->getMeshBase().numTris;
			oldLandmassVertHeights = new float[numVerts];

			for(int i = 0; i < numPlayers; i++)
				game->addPlayer(new Player(0, 0, 0, Vector3(1, 1, 1)));

			map->loadPlayersGameObjects();
		}
	}

	void MapEditorAppState::MapEditor::updateCircleRadius(bool increase){
		circleRadius += (increase ? INCREASE_RATE : -INCREASE_RATE);

		if(circleRadius > MAX_RADIUS)
			circleRadius = MAX_RADIUS;
		else if(circleRadius < MIN_RADIUS)
			circleRadius = MIN_RADIUS;
	}

	void MapEditorAppState::MapEditor::toggleSelection(Node *terrNode, bool select){
		terrNode->getMesh(0)->setWireframe(select);
		selectedTerrainNode = (select ? terrNode : nullptr);
	}

	void MapEditorAppState::MapEditor::castSelectionRay(){
	/*
		Camera *cam = Root::getSingleton()->getCamera();
		Vector3 startPos = cam->getPosition();
		Vector3 endPos = screenToSpace(getCursorPos());

		Map *map = Map::getSingleton();
		vector<RayCaster::CollisionResult> results = RayCaster::cast(startPos, (endPos - startPos).norm(), map->getNodeParent());

		if(selectedTerrainNode)
			toggleSelection(selectedTerrainNode, false);

		if(!results.empty()){
			for(int i = 0; i < map->getNodeParent()->getNumChildren(); i++)
				if(map->getNodeParent()->getChild(i) == results[i].mesh->getNode()){
					toggleSelection(map->getNodeParent()->getChild(i), true);
					break;
				}
		}
	*/
	}

	void MapEditorAppState::MapEditor::createWaterbody(){
		Material *mat = new Material(Root::getSingleton()->getLibPath() + "texture");
		mat->addBoolUniform("lightingEnabled", false);
		mat->addBoolUniform("texturingEnabled", true);
		mat->addTexUniform("textures[0]", waterTextures[0], false);

		Vector3 size = Vector3(30, 30, 1);
		Quad *quad = new Quad(size, true);
		quad->setMaterial(mat);

		Vector3 pos = 12 * Vector3::VEC_J;
		Node *node = new Node(pos);
		node->attachMesh(quad);

		Map *map = Map::getSingleton();
		Node *terrNodeParent = map->getNodeParent();
		terrNodeParent->attachChild(node);
		toggleSelection(node, true);
	}

	void MapEditorAppState::MapEditor::moveTerrainObject(float strength){
		if(fabs(strength) < .00001) return;

		Vector3 axis;

		switch(transformAxis){
			case TransformAxis::X_AXIS:
				axis = Vector3::VEC_I;
				break;
			case TransformAxis::Y_AXIS:
				axis = -Vector3::VEC_J;
				break;
			case TransformAxis::Z_AXIS:
				axis = Vector3::VEC_K;
				break;
		}

		Vector3 pos = selectedTerrainNode->getPosition() + 10 * strength * axis;
		selectedTerrainNode->setPosition(pos);
	}

	void MapEditorAppState::MapEditor::scaleTerrainObject(float strength){
		if(fabs(strength) < .00001) return;

		Vector2 axis;

		switch(transformAxis){
			case TransformAxis::X_AXIS:
				axis = Vector2::VEC_I;
				break;
			case TransformAxis::Z_AXIS:
				axis = Vector2::VEC_J;
				break;
		}

		Quad *quad = (Quad*)selectedTerrainNode->getMesh(0);
		Vector3 size = quad->getSize() + 10 * strength * Vector3(axis.x, axis.y, 0);
		quad->setSize(Vector3(size.x, size.y, 1));
		quad->updateVerts(quad->getMeshBase());
	}

	void MapEditorAppState::MapEditor::pushLandmassVerts(float strength){
		Mesh *mesh = map->getNodeParent()->getChild(0)->getMesh(0);
		MeshData meshData = mesh->getMeshBase();
		MeshData::Vertex *verts = meshData.vertices;
		int numVerts = meshData.numTris * 3;
		pushPos.y += 10 * strength;

		for(int i = 0; i < numVerts; i++){
			Vector3 distVec = *verts[i].pos - pushPos;
			distVec.y = 0;
			float dist = distVec.getLength();

			if(dist < circleRadius)
				verts[i].pos->y = oldLandmassVertHeights[i] + pushPos.y - (pushPos.y / circleRadius) * dist;
		}

		mesh->updateVerts(meshData);

		int minId = 0, maxId = 0;

		for(int i = 0; i < numVerts; i++){
			if(verts[i].pos->y < verts[minId].pos->y)
				minId = i;

			if(verts[i].pos->y > verts[maxId].pos->y)
				maxId = i;
		}

		Vector3 mapSize = map->getMapSize();
		map->setBaseHeight(verts[minId].pos->y);
		map->setMapSize(Vector3(mapSize.x, verts[maxId].pos->y - verts[minId].pos->y, mapSize.z));
	}

	void MapEditorAppState::MapEditor::generatePlane(Vector2 size){
		Root *root = Root::getSingleton();
		Material *mat = new Material(root->getLibPath() +  "texture");
		mat->addBoolUniform("texturingEnabled", false);
		mat->addBoolUniform("lightingEnabled", false);
		mat->addVec4Uniform("diffuseColor", Vector4(1, 0, 0, 1));

		Quad *mesh = new Quad(Vector3(size.x, size.y, 0), true, NUM_SUBDIVS, NUM_SUBDIVS);
		mesh->setMaterial(mat);

		Node *node = new Node();
		node->attachMesh(mesh);
		map->getNodeParent()->attachChild(node);

		oldLandmassVertHeights = new float[3 * mesh->getMeshBase().numTris];
	}

	void MapEditorAppState::MapEditor::prepareTextures(string basePath, bool skybox, vector<Texture*> &textures){
		AssetManager *assetManager = AssetManager::getSingleton();
		assetManager->load(basePath, skybox);
		vector<string> texturePaths;

		for(int i = 0; i < assetManager->getNumAssets(); i++){
			string assetPath = assetManager->getAsset(i)->path;

			if(assetPath.length() >= basePath.length() && assetPath.substr(0, basePath.length()) == basePath)
				texturePaths.push_back(assetPath);
		}

		if(skybox){
			const int numTex = 6;

			for(int i = 0; i < texturePaths.size() / numTex; i++){
				string paths[]{
					texturePaths[i * numTex],
					texturePaths[i * numTex + 1],
					texturePaths[i * numTex + 2],
					texturePaths[i * numTex + 3],
					texturePaths[i * numTex + 4],
					texturePaths[i * numTex + 5],
				};

				textures.push_back(new Texture(paths, numTex, true));
			}
		}
		else{
			for(int i = 0; i < texturePaths.size(); i++){
				string paths[]{texturePaths[i]};
				textures.push_back(new Texture(paths, 1, false));
			}
		}
	}

	void MapEditorAppState::MapEditor::generateLandmassXml(){
		XMLDocument *doc = new XMLDocument();

		char *nodeTagName = "node";
		XMLElement *rootEl = doc->NewElement(nodeTagName);
		rootEl->SetAttribute("name", "terrain");
		XMLNode *rootTag = doc->InsertEndChild(rootEl);

		XMLElement *nodeEl = doc->NewElement(nodeTagName);
		nodeEl->SetAttribute("name", "plane");
		Node *landmassNode = map->getNodeParent()->getChild(0);
		nodeEl->SetAttribute("px", landmassNode->getPosition().x);
		nodeEl->SetAttribute("py", landmassNode->getPosition().y);
		nodeEl->SetAttribute("pz", landmassNode->getPosition().z);
		nodeEl->SetAttribute("rw", landmassNode->getOrientation().w);
		nodeEl->SetAttribute("rx", landmassNode->getOrientation().x);
		nodeEl->SetAttribute("ry", landmassNode->getOrientation().y);
		nodeEl->SetAttribute("rz", landmassNode->getOrientation().z);
		nodeEl->SetAttribute("sx", landmassNode->getScale().x);
		nodeEl->SetAttribute("sy", landmassNode->getScale().y);
		nodeEl->SetAttribute("sz", landmassNode->getScale().z);
		XMLNode *nodeTag = rootTag->InsertEndChild(nodeEl);

		XMLElement *meshEl = doc->NewElement("mesh");
		MeshData meshData = map->getNodeParent()->getChild(0)->getMesh(0)->getMeshBase();
		meshEl->SetAttribute("name", "mesh");
		meshEl->SetAttribute("num_vertex_pos", 3 * meshData.numTris);
		meshEl->SetAttribute("num_faces", meshData.numTris);
		meshEl->SetAttribute("num_vertex_groups", 0);
		meshEl->SetAttribute("num_shape_keys", 0);
		XMLNode *meshTag = nodeTag->InsertEndChild(meshEl);

		//TODO optimize vertex position extraction
		/*
		const int numSideQuads = NUM_SUBDIVS + 1, numSideVerts = numSideQuads + 1;
		Vector2 subQuadSize = Vector2(size.x / numSideQuads, size.y / numSideQuads);
		*/
		int numVerts = 3 * meshData.numTris;

		for(int i = 0; i < numVerts; i++){
			XMLElement *vertEl = doc->NewElement("vertdata");
			vertEl->SetAttribute("px", meshData.vertices[i].pos->x);
			vertEl->SetAttribute("py", meshData.vertices[i].pos->y);
			vertEl->SetAttribute("pz", meshData.vertices[i].pos->z);
			/*
			vertEl->SetAttribute("nx", meshData.vertices[i].norm.x);
			vertEl->SetAttribute("ny", meshData.vertices[i].norm.y);
			vertEl->SetAttribute("nz", meshData.vertices[i].norm.z);
			 */
			XMLNode *vertNode = meshTag->InsertEndChild(vertEl);
		}

		for(int i = 0; i < numVerts; i++){
			XMLElement *vertEl = doc->NewElement("vert");
			vertEl->SetAttribute("id", i);
			vertEl->SetAttribute("uvx", meshData.vertices[i].uv.x);
			vertEl->SetAttribute("uvy", meshData.vertices[i].uv.y);
			vertEl->SetAttribute("tx", meshData.vertices[i].tan.x);
			vertEl->SetAttribute("ty", meshData.vertices[i].tan.y);
			vertEl->SetAttribute("tz", meshData.vertices[i].tan.z);
			vertEl->SetAttribute("bx", meshData.vertices[i].biTan.x);
			vertEl->SetAttribute("by", meshData.vertices[i].biTan.y);
			vertEl->SetAttribute("bz", meshData.vertices[i].biTan.z);
			XMLNode *vertTag = meshTag->InsertEndChild(vertEl);
		}

		string name = GameManager::getSingleton()->getPath() + "Models/Maps/" + map->getMapName() + "/" + map->getMapName() + ".xml";
		doc->SaveFile(name.c_str());
	}

	//TODO add diagnally adjacent edges to underwater cells 
	vector<Map::Cell> MapEditorAppState::MapEditor::generateMapCells(){
		Vector3 mapSize = map->getMapSize();
		Vector3 cellSize = map->getCellSize();
		Vector3 startPos = -.5 * (Vector3(mapSize.x, 0, mapSize.z) - Vector3(cellSize.x, 0, cellSize.z));

		int numHorCells = int(mapSize.x / cellSize.x);
		int numVertCells = int(mapSize.z / cellSize.z);
		vector<Map::Cell> cells;
		vector<pair<int, float>> waterBodyBedPoints;
		Node *terrainNode = map->getNodeParent();


		for(int i = 0; i < numVertCells; i++)
			for(int j = 0; j < numHorCells; j++){
				Vector3 rayPos = startPos + Vector3(cellSize.x * j, 100, cellSize.z * i);
				vector<RayCaster::CollisionResult> res = RayCaster::cast(rayPos, -Vector3::VEC_J, terrainNode->getChild(0), 0, configData::DIST_FROM_RAY);
				
				Map::Cell::Type type = Map::Cell::Type::LAND;
				Vector3 pos = res[0].pos;

				for(int k = 1; k < terrainNode->getNumChildren(); k++){
					Vector3 waterPos = terrainNode->getChild(k)->getPosition();
					Vector3 waterSize = ((Quad*)terrainNode->getChild(k)->getMesh(0))->getSize();

					if(fabs(res[0].pos.x - waterPos.x) < .5 * waterSize.x && fabs(res[0].pos.z - waterPos.z) < .5 * waterSize.y){
						type = Map::Cell::Type::WATER;
						pos.y = waterPos.y;
						waterBodyBedPoints.push_back(pair(numVertCells * i + j, res[0].pos.y));
						break;
					}
				}

				vector<Map::Edge> edges = Map::generateAdjacentNodeEdges(numVertCells, i, numHorCells, j, 10);
				Map::Cell cell = Map::Cell(pos, type, edges);
				cells.push_back(cell);
			}

		vector<Map::Cell> surfaceWaterCells;
		int currUnderWaterCellId = cells.size();
		int weight = 20;

		for(pair<int, float> p : waterBodyBedPoints){
			int numUnderWaterCells = (int)((cells[p.first].pos.y - p.second) / cellSize.y);

			if(numUnderWaterCells > 0){
				cells[p.first].edges.push_back(Map::Edge(weight, p.first, currUnderWaterCellId));

				for(int i = 0; i < numUnderWaterCells; i++, currUnderWaterCellId++)
					cells[p.first].underWaterCellIds.push_back(currUnderWaterCellId);

				surfaceWaterCells.push_back(cells[p.first]);
			}
		}

		for(int i = 0; i < surfaceWaterCells.size(); i++){
			for(int j = 0; j < surfaceWaterCells[i].underWaterCellIds.size(); j++){
				int aboveCellId = (j == 0 ? surfaceWaterCells[i].edges[0].srcCellId : surfaceWaterCells[i].underWaterCellIds[j - 1]);
				vector<Map::Edge> edges = vector<Map::Edge>{Map::Edge(weight, surfaceWaterCells[i].underWaterCellIds[j], aboveCellId)};

				if(surfaceWaterCells[i].underWaterCellIds.size() > j + 1)
					edges.push_back(Map::Edge(weight, surfaceWaterCells[i].underWaterCellIds[j], surfaceWaterCells[i].underWaterCellIds[j + 1]));

				for(int k = 0; k < surfaceWaterCells[i].edges.size() - 1; k++){
					Map::Cell adjacentUnderwaterCell = cells[surfaceWaterCells[i].edges[k].destCellId]; 

					if(adjacentUnderwaterCell.underWaterCellIds.size() >= j + 1){
						edges.push_back(Map::Edge(weight, surfaceWaterCells[i].underWaterCellIds[j], adjacentUnderwaterCell.underWaterCellIds[j]));
					}
				}

				Vector3 cellPos = surfaceWaterCells[i].pos - Vector3::VEC_J * cellSize.y * (j + 1);
				cells.push_back(Map::Cell(cellPos, Map::Cell::Type::WATER, edges));
			}
		}

		return cells;
	}

	void MapEditorAppState::MapEditor::generateMinimap(string mapFolder, vector<Map::Cell> &cells){
		Vector3 mapSize = map->getMapSize(), cellSize = map->getCellSize();
		int width = int(mapSize.x / cellSize.x);
		int height = int(mapSize.z / cellSize.z);
		int numChannels = 3;
		int size = width * height * numChannels;
		int cellId = 0;
		float baseHeight = map->getBaseHeight();

		u8 *imgData = new u8[size];

		for(u8 *p = imgData; p != imgData + size; p+= numChannels, cellId++){
			float min = .6, max = 1.;
			float heightFactor = min + (max - min) * (cells[cellId].pos.y - baseHeight) / mapSize.y;
			Vector3 color = (cells[cellId].type == Map::Cell::Type::WATER ? Vector3::VEC_K : Vector3::VEC_J);

			*p = color.x * heightFactor * 255.f;
			*(p + 1) = color.y * heightFactor * 255.f;
			*(p + 2) = color.z * heightFactor * 255.f;
		}

		stbi_write_jpg(string(mapFolder + "minimap.jpg").c_str(), width, height, numChannels, imgData, 100);
	}

	string MapEditorAppState::MapEditor::generatePlayerTableStr(Player* player){
		string mapScript = "";
		vector<ResourceDeposit*> resourceDeposits = player->getResourceDeposits();

		if(!resourceDeposits.empty()){
			mapScript += "\t\t\tresourceDeposits = {\n";

			for(ResourceDeposit *rd : resourceDeposits){
				Vector3 pos = rd->getPos();
				string posStr = "x = " + to_string(pos.x) + ", y = " + to_string(pos.y) + ", z = " + to_string(pos.z);

				Quaternion rot = rd->getRot();
				string rotStr = "w = " + to_string(rot.w) + ", x = " + to_string(rot.x) + ", y = " + to_string(rot.y) + ", z = " + to_string(rot.z);

				mapScript += "\t\t\t\t{id = " + to_string(rd->getId()) + ", pos = {" + posStr + "}, rot = {" + rotStr + "}},\n";
			}

			mapScript += "\t\t\t},\n";
		}

		vector<Unit*> units = player->getUnits();

		if(!units.empty()){
			mapScript += "\t\t\tunits = {\n";

			for(Unit *unit : units){
				string idStr = "id = " + to_string(unit->getId());

				Vector3 unitPos = unit->getPos();
				string posStr = "pos = {x = " + to_string(unitPos.x) + ", y = " + to_string(unitPos.y) + ", z = " + to_string(unitPos.z) + "}";

				Quaternion unitRot = unit->getRot();
				string rotStr = "rot = {w = " + to_string(unitRot.w) + ", x = " + to_string(unitRot.x) + ", y = " + to_string(unitRot.y) + ", z = " + to_string(unitRot.z) + "}";

				mapScript += "\t\t\t\t{" + idStr + ", " + posStr + ", " + rotStr + "},\n";
			}

			mapScript += "\t\t\t}\n";
		}

		return mapScript;
	}

	void MapEditorAppState::MapEditor::generateMapScript(vector<Map::Cell> &cells){
		int numWaterBodies = map->getNodeParent()->getNumChildren() - 1;

		string mapScript = "map = {\n\tlights = {";

		for(Node *light : map->getLights()){
			mapScript += "\n\t\t{type = " + to_string((int)light->getLight(0)->getLightType());

			if(light->getLight(0)->getLightType() == Light::Type::DIRECTIONAL){
				Vector3 dir = light->getGlobalAxis(2);
				mapScript += ", dir = {x = " + to_string(dir.x) + ", y = " + to_string(dir.y) + "z = " + to_string(dir.z) + "}";
			}

			Vector3 color = light->getLight(0)->getColor();
			mapScript += ", color = {x = " + to_string(color.x) + ", y = " + to_string(color.y) + ", z = " + to_string(color.z) + "}},";
		}

		mapScript += "\n\t},\n";
		Vector3 mapSize = map->getMapSize();
		mapScript += "\tsize = {x = " + to_string(mapSize.x) + ", y = " + to_string(mapSize.y) + ", z = " + to_string(mapSize.z) + "},\n";
		mapScript += "\timpassibleNodeValue = " + to_string(IMPASS_NODE_VAL) + ",\n";

		int numSpawnPoints = map->getNumSpawnPoints();
		mapScript += "\tspawnPoints = {\n";

		for(int i = 0; i < numSpawnPoints; i++){
			Vector3 sp = map->getSpawnPoint(i);
			mapScript += "\t\t{x = " + to_string(sp.x) + ", y = " + to_string(sp.y) + ", z = " + to_string(sp.z) + "},\n";
		}

		mapScript += "\t},\n";
		mapScript += "\tchoosablePlayers = {";
		Game *game = Game::getSingleton();

		for(Player *player : game->getPlayers())
			mapScript += "\n\t\t{\n" + generatePlayerTableStr(player) + "\n\t\t},\n";

		mapScript += "\t},\n";
		mapScript += "\tcivilianPlayer = {\n" + generatePlayerTableStr(game->getCivilianPlayer()) + "\n\t},\n";
		mapScript += "\tcells = {\n";

		for(Map::Cell cell : cells){
			Vector3 p = cell.pos;
			mapScript += "\t\t{type = " + to_string((int)cell.type) + ", pos = {x = " + to_string(p.x) + ", y = " + to_string(p.y) + ", z = " + to_string(p.z) + "}, numEdges = " + to_string(cell.edges.size()) + ", edges = {";

			for(Map::Edge edge : cell.edges)
				mapScript += "{srcCellId = " + to_string(edge.srcCellId) + ", destCellId = "  + to_string(edge.destCellId) + ", weight = "  + to_string(edge.weight) + "}, ";

			int numSubCells = cell.underWaterCellIds.size();
			mapScript += "}, numUnderWaterCells = " + to_string(numSubCells) + ",";

			if(numSubCells > 0){
				mapScript += "underWaterCellId = {";

				for(int subCellId : cell.underWaterCellIds)
					mapScript += to_string(subCellId) + ", ";

				mapScript += "}";
			}

			mapScript += "\t\t},\n";
		}

		mapScript += "\t},\n";

		Root *root = Root::getSingleton();
		string skyboxPath = "";

		if(root->getSkybox()){
			Texture *tex = ((Material::TextureUniform*)root->getSkybox()->getMaterial()->getUniform("tex"))->value;
			string path = tex->getPath()[0];
			int slashId = path.find_last_of('/');
			skyboxPath = path.substr(0, slashId);

			slashId = skyboxPath.find_last_of('/');
			skyboxPath = skyboxPath.substr(slashId + 1);
		}

		mapScript += "\tskybox = \"" + skyboxPath + "\",\n";
		mapScript += "\tterrain = {model = \"" + map->getMapName()  + ".xml\", albedo = \"" + map->getMapName() + ".jpg\"},\n";
		mapScript += "\twaterbodies = {\n";

		for(int i = 0; i < numWaterBodies; i++){
			Node *waterNode = map->getNodeParent()->getChild(i + 1);
			Vector3 pos = waterNode->getPosition();
			Vector3 size = ((Quad*)waterNode->getMesh(0))->getSize();
			mapScript += 
				"\t\t{pos = {x = " + to_string(pos.x) + ", y = " + to_string(pos.y) + ", z = " + to_string(pos.z) + "},\
				size = {x = " + to_string(size.x) + ", y = " + to_string(size.y) + "}, albedo = \"water.png\"},";
		}

		mapScript += "\t}\n}";

		string file = GameManager::getSingleton()->getPath() + "Models/Maps/" + map->getMapName() + "/" + map->getMapName() + ".lua";

		std::ofstream outFile(file);
		outFile << mapScript;
		outFile.close();
	}

	void MapEditorAppState::MapEditor::exportMap(){
		string assetsPath = GameManager::getSingleton()->getPath();
		string mapFolder = assetsPath + "Models/Maps/" + map->getMapName() + "/";
		create_directory(mapFolder);
		copy_file(assetsPath + DEFAULT_TEXTURE, mapFolder + map->getMapName() + ".jpg", filesystem::copy_options::overwrite_existing);

		vector<Map::Cell> cells = generateMapCells();
		generateLandmassXml();
		generateMapScript(cells);
		generateMinimap(mapFolder, cells);
	}

	void MapEditorAppState::MapEditor::togglePush(bool push){
		this->pushing = push;

		if(push){
			MeshData meshData = map->getNodeParent()->getChild(0)->getMesh(0)->getMeshBase();
			int numVerts = 3 * meshData.numTris;

			for(int i = 0; i < numVerts; i++)
				oldLandmassVertHeights[i] = meshData.vertices[i].pos->y;
		}
	}

	MapEditorAppState::MapEditorAppState(string name, Vector2 size, bool newMap) : AbstractAppState(
					AppStateType::MAP_EDITOR,
				 	configData::calcSumBinds(AppStateType::MAP_EDITOR, true),
				 	configData::calcSumBinds(AppStateType::MAP_EDITOR, false),
				 	GameManager::getSingleton()->getPath() + scripts[ScriptFiles::OPTIONS]){
		mapName = name;
		mapSize = size;
		this->newMap = newMap;
	}

	void MapEditorAppState::update(){
		radiusText->setText(L"Radius: " + to_wstring(mapEditor->getCircleRadius()));
		weightsText->setText(L"Weights generated: " + to_wstring(mapEditor->isWeightsGenerated()));

		CameraController *camCtr = CameraController::getSingleton();

		if(!(camCtr->isLookingAround() || mapEditor->isPushing()))
			camCtr->updateCameraPosition();

		GameObjectFrameController *ufCtr = GameObjectFrameController::getSingleton();

		if(ufCtr->isPlacingOnSurface())
			ufCtr->update();
	}

	void MapEditorAppState::onAttached(){
		AbstractAppState::onAttached();
		mapEditor = new MapEditor(mapName, mapSize, newMap);

		Root *root = Root::getSingleton();
		Material *mat = new Material(root->getLibPath() + "text");
		mat->addBoolUniform("texturingEnabled", false);
		mat->addVec4Uniform("diffuseColor", Vector4::VEC_IJKL);

		string fontPath = GameManager::getSingleton()->getPath() + "Fonts/batang.ttf";
		radiusText = new Text(fontPath, L"");
		radiusText->setMaterial(mat);
		weightsText = new Text(fontPath, L"");
		weightsText->setMaterial(mat);

		Node *radiusNode = new Node(Vector3(0, 100, 0));
		radiusNode->addText(radiusText);
		root->getGuiNode()->attachChild(radiusNode);

		Node *weightsNode = new Node(Vector3(0, 200, 0));
		weightsNode->addText(weightsText);
		root->getGuiNode()->attachChild(weightsNode);
	}

	void MapEditorAppState::onDettached(){}

	void MapEditorAppState::onAction(int bind, bool isPressed){
		GameObjectFrameController *ufCtr = GameObjectFrameController::getSingleton();

		switch((Bind)bind){
			case Bind::LOOK_AROUND:
				if(!ufCtr->isPlacingOnSurface())
					CameraController::getSingleton()->setLookingAround(isPressed);

                break;
			case Bind::ROTATE_OBJ_FRAME:
				if(isPressed && ufCtr->isPlacingOnSurface())
					ufCtr->setRotating(true);
				else if(!isPressed && ufCtr->isRotating()){
					Player *player = Game::getSingleton()->getPlayer(0);
					GameObjectFrame &frame = ufCtr->getGameObjectFrame(0);
					Model *model = frame.getModel();
					Vector3 pos = model->getPosition();
					Quaternion rot = model->getOrientation();

					if(frame.getType() == GameObject::Type::RESOURCE_DEPOSIT)
						player->addResourceDeposit(GameObjectFactory::createResourceDeposit(player, frame.getId(), pos, rot));
					else
						player->addUnit(GameObjectFactory::createUnit(player, frame.getId(), pos, rot));

					ufCtr->setRotating(false);
				}

				break;
			case Bind::DESELECT_STRUCTURE:
				ufCtr->removeGameObjectFrames();
				ufCtr->setPlacingOnSurface(false);
				ufCtr->setRotating(false);
				break;
			case Bind::INCREASE_RADIUS:
				if(isPressed) mapEditor->updateCircleRadius(true);
				break;
			case Bind::DECREASE_RADIUS:
				if(isPressed) mapEditor->updateCircleRadius(false);
				break;
			case Bind::LEFT_CLICK:
				if(isPressed){
					Camera *cam = Root::getSingleton()->getCamera();
					Vector3 startPos = cam->getPosition();
					Vector2 cursorPos = getCursorPos();
					Vector3 endPos = screenToSpace(cursorPos);

					vector<RayCaster::CollisionResult> results = RayCaster::cast(
							startPos, 
							(endPos - startPos).norm(), 
							Root::getSingleton()->getRootNode(),
						   	0, 
							configData::DIST_FROM_RAY
					);

					if(cursorPos.y < GameManager::getSingleton()->getHeight() - mapEditor->getGuiThreshold()){
						bool push = !results.empty();
						mapEditor->togglePush(push);

						if(push)
							mapEditor->setPushPos(results[0].pos);

						mapEditor->castSelectionRay();
					}
					else
						mapEditor->togglePush(false);
				}
				else
					mapEditor->togglePush(false);

				break;
			case Bind::CREATE_WATERBODY:
				if(isPressed) mapEditor->createWaterbody();
				break;
			case Bind::MOVE_TERR_OBJ:
				if(isPressed){
					mapEditor->setMovingTerrainObject(true);
					mapEditor->setScalingTerrainObject(false);
					break;
				}
			case Bind::SCALE_TERR_OBJ:
				if(isPressed){
					mapEditor->setMovingTerrainObject(false);
					mapEditor->setScalingTerrainObject(true);
					break;
				}
			case Bind::STOP_TERR_OBJ:
				if(isPressed){
					mapEditor->setMovingTerrainObject(false);
					mapEditor->setScalingTerrainObject(false);
					break;
				}
			case Bind::ENABLE_X_AXIS:
			case Bind::ENABLE_Y_AXIS:
			case Bind::ENABLE_Z_AXIS:
				if(isPressed && (mapEditor->isMovingTerrainObject() || mapEditor->isScalingTerrainObject())){
					MapEditor::TransformAxis axis = MapEditor::TransformAxis((int)bind - (int)Bind::ENABLE_X_AXIS);
					mapEditor->setTransformAxis(axis);
				}

				break;
		}
	}

	void MapEditorAppState::onAnalog(int bind, float strength){
		CameraController *camCtr = CameraController::getSingleton();
		GameObjectFrameController *ufCtr = GameObjectFrameController::getSingleton();

		switch((Bind)bind){
			case Bind::LOOK_UP: 
			case Bind::LOOK_DOWN: 
				if(camCtr->isLookingAround()){
					Vector3 dirProj = Root::getSingleton()->getCamera()->getDirection();
					dirProj = Vector3(dirProj.x, 0, dirProj.z).norm();
					camCtr->orientCamera(Vector3(0, 1, 0).cross(dirProj), strength);
				}

				break;
			case Bind::LOOK_LEFT: 
			case Bind::LOOK_RIGHT: 
				if(camCtr->isLookingAround())
					camCtr->orientCamera(Vector3(0, 1, 0), strength);
				else if(ufCtr->isRotating())
					ufCtr->rotateGameObjectFrames(100 * strength);

				break;
			case Bind::PUSH_VERTS_UP:
			case Bind::PUSH_VERTS_DOWN:
				if(mapEditor->isPushing()) mapEditor->pushLandmassVerts(strength);
				else if(mapEditor->getSelectedNode() != Map::getSingleton()->getNodeParent()->getChild(0)){
					if(mapEditor->isScalingTerrainObject())
						mapEditor->scaleTerrainObject(strength);
					else if(mapEditor->isMovingTerrainObject())
						mapEditor->moveTerrainObject(strength);
				}

				break;
		}
	}

	void MapEditorAppState::onRawMouseWheelScroll(bool up){
		CameraController::getSingleton()->zoomCamera(up);
	}
}
