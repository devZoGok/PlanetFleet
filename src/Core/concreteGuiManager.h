#ifndef CONCRETE_GUI_MANAGER_H
#define CONCRETE_GUI_MANAGER_H

#include <abstractGuiManager.h>

#include <string>
#include <utility>
#include <vector>

namespace vb01{
	class Node;
	class Text;
}

namespace battleship{
	enum GuiElementType {BUTTON, LISTBOX, CHECKBOX, SLIDER, TEXTBOX, GUI_RECTANGLE, TEXT, MUSIC};
	enum ButtonType {
		SINGLE_PLAYER,
	   	EDITOR,
	   	OPTIONS,
	   	EXIT,
		OK,
		DEFAULTS,
		BACK,
		CONTROLS_TAB,
		MOUSE_TAB,
		VIDEO_TAB,
		AUDIO_TAB,
		MULTIPLAYER_TAB,
		NEW_MAP,
		NEW_MAP_OK,
		LOAD_MAP,
		LOAD_MAP_OK,
		EXPORT,
		PLAY,
		RESUME,
		CONSOLE_SCREEN,
		MAIN_MENU,
		CONSOLE_COMMAND_OK,
		BUILD,
		TRAIN,
		STATISTICS,
		RESEARCH,
		BUY_REFINEDS,
		SELL_REFINEDS,
		BUY_RESEARCH,
		SELL_RESEARCH,
		ACTIVE_STATE_BUTTON,
		ACTIVE_STATE_BACK,
		PLAYER_TRADE,
		TRADING_SCREEN,
		TRADE_OFFER,
		RESOURCE_AMMOUNT,
		UNIT_STATE,
		ORDER,
		MINIMAP,
	};
	enum ListboxType {
		CONTROLS,
		RESOLUTION,
		MAPS,
		VEHICLES,
		STRUCTURES,
		RESOURCE_DEPOSITS,
		SKYBOX_TEXTURES,
		LAND_TEXTURES,
		CPU_DIFFICULTIES,
		FACTIONS,
		COLORS,
		TEAMS,
		CONSOLE,
		TRADE_OFFERS
	};

	class ConcreteGuiManager : public vb01Gui::AbstractGuiManager{
		public:
			static ConcreteGuiManager* getSingleton();
			void readLuaScreenScript(
					std::string,
					std::vector<vb01Gui::Button*> = std::vector<vb01Gui::Button*>{},
					std::vector<vb01Gui::Listbox*> = std::vector<vb01Gui::Listbox*>{},
					std::vector<vb01Gui::Checkbox*> = std::vector<vb01Gui::Checkbox*>{},
					std::vector<vb01Gui::Slider*> = std::vector<vb01Gui::Slider*>{},
					std::vector<vb01Gui::Textbox*> = std::vector<vb01Gui::Textbox*>{},
					std::vector<vb01::Node*> = std::vector<vb01::Node*>{},
					std::vector<vb01::Text*> = std::vector<vb01::Text*>{},
					std::string = ""
				);
			void readLuaScreenScriptDel(
					std::string,
					std::vector<vb01Gui::Button*> = std::vector<vb01Gui::Button*>{},
					std::vector<vb01Gui::Listbox*> = std::vector<vb01Gui::Listbox*>{},
					std::vector<vb01Gui::Checkbox*> = std::vector<vb01Gui::Checkbox*>{},
					std::vector<vb01Gui::Slider*> = std::vector<vb01Gui::Slider*>{},
					std::vector<vb01Gui::Textbox*> = std::vector<vb01Gui::Textbox*>{},
					std::vector<vb01::Node*> = std::vector<vb01::Node*>{},
					std::vector<vb01::Text*> = std::vector<vb01::Text*>{}
				);
			void parseLuaScript(std::string, std::string = "");
		private:
			ConcreteGuiManager();
			vb01Gui::Button* parseButton(int);
			vb01Gui::Listbox* parseGameObjectListbox();
			vb01Gui::Listbox* parseListbox(int);
			vb01Gui::Checkbox* parseCheckbox(int);
			vb01Gui::Slider* parseSlider(int);
			vb01Gui::Textbox* parseTextbox(int);
			vb01::Node* parseGuiRectangle(int);
			vb01::Text* parseText(int);
			void parseMusic();

			std::vector<std::pair<int*, void*>> guiElements;
			std::string texBasePath, fontBasePath;
	};
}

#endif
