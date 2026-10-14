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

	struct CellId{
		CellId(){}
		CellId(vb01::u16 xx, vb01::u16 yy, vb01::u16 zz) : x(xx), y(yy), z(zz){}
		bool operator==(const CellId &cid){return x == cid.x && y == cid.y && z == cid.z;}
		bool operator!=(const CellId &cid){return !(x == cid.x && y == cid.y && z == cid.z);}

		vb01::u16 x, y, z;
	};

    class Map {
    public:
		struct Cell;
		
		struct Edge{
			Edge(vb01::s64 w, CellId dest) : weight(w), destCellId(dest){}
			vb01::s64 weight;
			CellId destCellId;
		};
		
		struct Cell{
			enum Type{LAND, WATER};
		
			Type type;
			vb01::Vector3 pos;
			std::vector<Edge> edges;
			Unit *blockedBy = nullptr;
			int numUnderWaterCells = 0;
		
			Cell(){}
			Cell(vb01::Vector3 p, Type t, std::vector<Edge> e = std::vector<Edge>{}, int nuwc = 0) : pos(p), type(t), edges(e), numUnderWaterCells(nuwc){}
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
        void update();
        void load(std::string);
        void create(std::string, vb01::Vector3);
        void unload();
		std::vector<vb01::RayCaster::CollisionResult> raycastTerrain(vb01::Vector3, vb01::Vector3, bool);
		CellId getCellId(vb01::Vector3, bool = true, int = 1, int = 1, int = 1);
		bool isPointWithinTerrainObject(vb01::Vector3, int);
		void loadPlayersGameObjects();
		int getNumMapSpawnPoints(std::string = "");
		std::vector<int> getSurroundingCells(vb01::Vector3, int);
		void blockCells(Unit*);
		void unblockCells(Unit*);
		CellId getNumCells(int = 1, int = 1, int = 1);
		Map::Cell getCell(CellId, int = 1, int = 1, int = 1);
		Map::Cell getCell(int, int, int, int = 1, int = 1, int = 1);
		inline std::string getMapName(){return mapName;}
		inline vb01::Node* getNodeParent(){return terrainNode;}
		inline vb01::Vector3 getCellSize(){return CELL_SIZE;}
		inline int getNumSpawnPoints(){return spawnPoints.size();}
		inline vb01::Vector3 getSpawnPoint(int i){return spawnPoints[i];}
		inline void setMapSize(vb01::Vector3 s){this->mapSize = s;}
		inline vb01::Vector3 getMapSize(){return mapSize;}
		inline void addSpawnPoint(vb01::Vector3 sp){spawnPoints.push_back(sp);}
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
		float baseHeight;
		std::vector<vb01::Node*> lights;

        Map(){}
		void preprareScene(bool);
		void loadSpawnPoints();
		void loadLights();
		void loadSkybox();
		void loadPlayerGameObjects(Player*, sol::table);
		void loadTerrainObject(int);
		void unloadTerrainObjects();
		void unloadLights();
		void unloadSkybox();
		void unloadPlayerObjects();
		void destroyScene();
		template<typename T> int bsearch(std::vector<T>, T, float);
    };
}
#endif
