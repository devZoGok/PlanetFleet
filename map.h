#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>

#include <rayCaster.h>
#include <vector.h>
#include <util.h>

#include <solUtil.h>

namespace vb01{
	class Model;
	class Material;
	class Node;
}

namespace vb01Gui{
	class Button;
}

namespace battleship{
	class Player;
	class Unit;
	class ResourceDeposit;
	struct Cell;

    class Map {
    public:
		struct Cell;
		
		struct Edge{
			Edge(vb01::s64 w, vb01::s64 src, vb01::s64 dest) : weight(w), srcCellId(src), destCellId(dest){}
			vb01::s64 weight, srcCellId, destCellId;
		};
		
		struct Cell{
			enum Type{LAND, WATER};
		
			Type type;
			vb01::Vector3 pos;
			std::vector<Edge> edges;
			Unit *blockedBy = nullptr;
			std::vector<int> underWaterCellIds;
		
			Cell(){}
			Cell(vb01::Vector3 p, Type t, std::vector<Edge> e = std::vector<Edge>{}, std::vector<int> uc = std::vector<int>{}): pos(p), type(t), edges(e), underWaterCellIds(uc){}
		};

		class Minimap{
			public:
				static Minimap* getSingleton();
				~Minimap();
				void update();
				void updateImage();
				void load();
				void unload();
				inline vb01::u8* getOldMinimapImage(){return oldImageData;}
			private:
				Minimap();
				vb01::Node* initIcon(vb01::Vector3, std::string);
				void updateCamFrame(vb01Gui::Button*);

				vb01::u8 *oldImageData = nullptr;
				vb01::Node *camFrame = nullptr;
				std::vector<vb01::Node*> depositIcons;
				vb01::Node* camIcon = nullptr;
		};

		static Map* getSingleton();
        ~Map(){}
		static std::vector<Edge> generateAdjacentNodeEdges(int, int, int, int, int);
        void update();
        void load(std::string);
        void create(std::string, vb01::Vector3);
        void unload();
		std::vector<vb01::RayCaster::CollisionResult> raycastTerrain(vb01::Vector3, vb01::Vector3, bool);
		int getCellId(vb01::Vector3, bool = true);
		bool isPointWithinTerrainObject(vb01::Vector3, int);
		void loadPlayersGameObjects();
		int getNumMapSpawnPoints(std::string = "");
		std::vector<int> getSurroundingCells(vb01::Vector3, int);
		void blockCells(Unit*);
		void unblockCells(Unit*);
		inline Map::Cell getCell(int i){return cells[i];}
		inline std::string getMapName(){return mapName;}
		inline vb01::Node* getNodeParent(){return terrainNode;}
		inline vb01::Vector3 getCellSize(){return CELL_SIZE;}
		inline int getNumSpawnPoints(){return spawnPoints.size();}
		inline vb01::Vector3 getSpawnPoint(int i){return spawnPoints[i];}
		inline void setMapSize(vb01::Vector3 s){this->mapSize = s;}
		inline vb01::Vector3 getMapSize(){return mapSize;}
		inline void addSpawnPoint(vb01::Vector3 sp){spawnPoints.push_back(sp);}
		inline std::vector<Map::Cell>& getCells(){return cells;}
		inline vb01::Node* getLight(int i){return lights[i];}
		inline std::vector<vb01::Node*> getLights(){return lights;}
		inline float getBaseHeight(){return baseHeight;}
		inline void setBaseHeight(float bh){this->baseHeight = bh;}
    private:
		std::string mapTable = "metadata";
		vb01::Node *terrainNode = nullptr, *cellNode = nullptr;
		vb01::Material *landCellMat = nullptr, *waterCellMat = nullptr;
		std::string mapName;
		vb01::Vector3 CELL_SIZE = vb01::Vector3(7, 7, 7), mapSize;
		std::vector<vb01::Vector3> spawnPoints;
		std::vector<Cell> cells;
		float baseHeight;
		std::vector<vb01::Node*> lights;

        Map(){}
		void preprareScene(bool);
		void loadSpawnPoints();
		void loadLights();
		void loadSkybox();
		void loadCells();
		void loadPlayerGameObjects(Player*, sol::table);
		void loadTerrainObject(int);
		void unloadTerrainObjects();
		void unloadCells();
		void unloadLights();
		void unloadSkybox();
		void unloadPlayerObjects();
		void destroyScene();
		template<typename T> int bsearch(std::vector<T>, T, float);
    };
}
#endif
