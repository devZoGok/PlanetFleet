#ifndef MAP_EDITOR_APP_STATE_H
#define MAP_EDITOR_APP_STATE_H

#include "map.h"
#include "player.h"

#include <listbox.h>

#include <abstractAppState.h>

#include <vector.h>
#include <util.h>

#include <string>

namespace vb01{
	class Texture;
}

namespace battleship{
	class MapEditor;

	class MapEditorAppState : public gameBase::AbstractAppState{
		public:
			class MapEditor{
				public:
					enum TransformAxis{
						X_AXIS,
						Y_AXIS,
						Z_AXIS
					};

					MapEditor(std::string, vb01::Vector2, bool);
					void updateCircleRadius(bool);
					void pushLandmassVerts(float);
					void castSelectionRay();
					void createWaterbody();
					void moveTerrainObject(float);
					void scaleTerrainObject(float);
					void exportMap();
					void prepareTerrainObjects(int = 0, int = -1);
					void togglePush(bool);
					inline vb01::Node* getSelectedNode(){return selectedTerrainNode;}
					inline float getGuiThreshold(){return guiThreshold;}
					inline float getCircleRadius(){return circleRadius;}
					inline bool isPushing(){return pushing;}
					inline void setPushPos(vb01::Vector3 p){pushPos = p;}
					inline bool isMovingTerrainObject(){return movingTerrainObject;}
					inline void setMovingTerrainObject(bool m){movingTerrainObject = m;}
					inline bool isScalingTerrainObject(){return scalingTerrainObject;}
					inline void setScalingTerrainObject(bool s){scalingTerrainObject = s;}
					inline TransformAxis getTransformAxis(){return transformAxis;}
					inline void setTransformAxis(TransformAxis m){transformAxis = m;}
					inline bool isWeightsGenerated(){return weightsGenerated;}
					inline vb01::Texture* getSkyTexture(int i){return skyTextures[i];}
					inline vb01::Texture* getLandmassTexture(int i){return landmassTextures[i];}
				private:
					void generatePlane(vb01::Vector2);
					void prepareTextures(std::string, bool, std::vector<vb01::Texture*>&);
					void toggleSelection(vb01::Node*, bool);
					std::vector<Map::Cell> generateMapCells();
					void generateLandmassXml();
					void generateMinimap(std::string, std::vector<Map::Cell>&);
					std::string generatePlayerTableStr(Player*);
					void generateMapScript(std::vector<Map::Cell>&);
					void prepareTerrainObject(vb01::u32**, Map::Cell*, int[3], float, bool);

					Map *map;
					vb01::Node *selectedTerrainNode = nullptr;
					TransformAxis transformAxis = X_AXIS;
					vb01Gui::Listbox *skyListbox = nullptr;
					float *oldLandmassVertHeights = nullptr;
					bool pushing = false, movingTerrainObject = false, scalingTerrainObject = false, weightsGenerated = false, newMap, cellMarkersVisible = false;
					const float MIN_RADIUS = 1, MAX_RADIUS = 100, INCREASE_RATE = 1;
					const int NUM_SUBDIVS = 100;
					float circleRadius = MIN_RADIUS, guiThreshold = 200;
					vb01::Vector3 pushPos = vb01::Vector3::VEC_ZERO;
					std::vector<vb01::Texture*> skyTextures, landmassTextures, waterTextures;
			};

			MapEditorAppState(std::string, vb01::Vector2, bool);
			void update();
			void onAttached();
			void onDettached();
			void onAction(int, bool);
			void onAnalog(int, float);
			void onRawMouseWheelScroll(bool);
			inline MapEditor* getMapEditor(){return mapEditor;}
		private:
			MapEditor *mapEditor = nullptr;
			std::string mapName;
			vb01::Vector2 mapSize;
			bool newMap;
			vb01::Text *radiusText = nullptr, *weightsText = nullptr;
	};
}

#endif
