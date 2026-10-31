#pragma once
#ifndef IN_GAME_APP_STATE_H
#define IN_GAME_APP_STATE_H

#include "gameManager.h"
#include "player.h"
#include "map.h"
#include "activeGameState.h"
#include "guiAppState.h"
#include "exitButton.h"
#include "optionsButton.h"

#include <vector>

//TODO refactor dependence on ActiveGameState
namespace battleship{
    class InGameAppState : public gameBase::AbstractAppState {
    public:
        class ResumeButton : public PfButtonBase {
        public:
            ResumeButton(vb01::Vector3, vb01::Vector2);
            void onClick();
        private:
        };

        class ConsoleButton : public PfButtonBase {
        public:
			class ConsoleCommandEntryButton : public PfButtonBase {
			public:
			    ConsoleCommandEntryButton(vb01Gui::Textbox*, vb01Gui::Listbox*, vb01::Vector3, vb01::Vector2, std::string);
			    void onClick();
			private:
			    vb01Gui::Textbox *textbox;
			    vb01Gui::Listbox *listbox;
			};

            ConsoleButton(vb01::Vector3, vb01::Vector2);
            void onClick();
        private:
        };

        InGameAppState(std::string);
        ~InGameAppState(){}
        void onAttached();
        void onDettached();
        void update();
        void onAction(int, bool);
        void onAnalog(int, float);
        std::vector<Unit*> getSelectedUnits(Player*);
		inline ActiveGameState* getActiveState(){return activeState;}
    private:
        bool isMainMenuActive = false;
        std::vector<std::string> modelPaths;
        int playerId;
		std::string mapName = "";
        ActiveGameState* activeState;
    };
}

#endif
