#ifndef ACTIVE_GAME_STATE_H
#define ACTIVE_GAME_STATE_H

#include "guiAppState.h"
#include "player.h"
#include "map.h"
#include "unit.h"

#include <util.h>

namespace vb01{
	class Node;
	class Texture;
}

namespace vb01Gui{
	class Button;
}

namespace battleship{
	class GameObject;
	class Unit;
	class UnitButton;

    class ActiveGameState : public gameBase::AbstractAppState {
    public:
		enum CursorState{
			NORMAL,
			ATTACK,
			GARRISON,
			SUPPLY,
			LOAD,
			UNLOAD,
			HACK
		};

        ActiveGameState(GuiAppState*, int);
        ~ActiveGameState();
        void onAttached();
        void onDettached();
        void update();
        void onAction(int, bool);
        void onAnalog(int, float);
		void onRawMouseWheelScroll(bool);
		inline CursorState getCursorState(){return cursorState;}
		inline void setCursorState(CursorState cs){this->cursorState = cs;}
		inline void addButton(vb01Gui::Button *b){guiButtons.push_back(b);}
		inline std::vector<vb01Gui::Button*> getGuiButtons(){return guiButtons;}
		inline std::vector<vb01::Node*> getGuiRects(){return guiRects;}
		inline std::vector<vb01::Text*> getGuiTexts(){return guiTexts;}
        inline Player* getPlayer(){return mainPlayer;}
        inline std::vector<Unit*>& getUnitGroup(int i){return unitGroups[i];}
		inline void setBuildableStructSelected(bool bss){this->buildableStructSelected = bss;}
		inline bool isBuildableStructSelected(){return buildableStructSelected;}
		inline float getDepth(){return depth;}
		inline void setForceCursorState(bool force){this->forceCursorState = force;}
		inline bool isForceCursorState(){return forceCursorState;}
		inline void setTradingScreen(bool ts){this->tradingScreen = ts;}
		inline bool isTradingScreen(){return tradingScreen;}
		inline void setOfferScreen(bool os){this->offerScreen = os;}
		inline bool isOfferScreen(){return offerScreen;}
    private:
		bool selectedUnitsAmongst(std::vector<Unit*>);
		void updateGameObjHoveredOn();
		void initCursor();
		void updateCursor();
		void initDragbox();
		void removeDragbox();
		bool isGameObjSelectable(GameObject*, bool);
		void deselectUnits();
        void renderUnits();
        void updateDragBox();
		void updateStructureFrames();
        void castRayToTerrain();
        void issueOrder(Order::TYPE, std::vector<Order::Target>, bool);
		void enableUnitState(Unit::State);
		void addUnitGui();
		inline bool canSelectHoveredOnGameObj(){return gameObjHoveredOn && gameObjHoveredOn->isSelectable() && gameObjHoveredOn->getPlayer() == mainPlayer;}

		CursorState cursorState = CursorState::NORMAL;
        Player *mainPlayer;
        GuiAppState *guiState;
		GameObject *gameObjHoveredOn = nullptr;
		vb01::Vector2 clickPoint;
        std::vector<Unit*> unitGroups[9], prevSelectedUnits;
		std::vector<Order::Target> targets;
		vb01::Node *dragboxNode = nullptr;
		std::vector<vb01::Node*> guiRects;
		std::vector<vb01::Text*> guiTexts;
		std::vector<vb01Gui::Button*> guiButtons, unitButtons;
        bool isSelectionBox = false;
		bool shiftPressed = false;
		bool controlPressed = false;
		bool selectMouseClicked = false;
		bool orderMouseClicked = false;
		bool buildableStructSelected = false;
		bool selectingPatrolPoints = false;
		bool selectingDestOrient = false;
		bool forceCursorState = false;
		bool orderPossible = false;
		bool tradingScreen = false;
		bool offerScreen = false;
        int playerId, zooms = 0;
	   	const int NUM_MAX_ZOOMS = 10;
		float depth = 1;
		vb01::s64 lastSelectMouseClicked = 0, lastOrderMouseClicked = 0;
		vb01::Node *cursorNode = nullptr;
		vb01::Texture *pointerTex = nullptr, *attackTex = nullptr, *garrisonTex = nullptr, *noGarrisonTex = nullptr, *supplyTex = nullptr, *noSupplyTex = nullptr, *hackTex = nullptr, *noHackTex = nullptr;
    };
}

#endif
