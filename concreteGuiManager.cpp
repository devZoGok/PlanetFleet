#include <soundManager.h>

#include <quad.h>

#include "concreteGuiManager.h"
#include "unit.h"
#include "gameManager.h"
#include "pfButtonBase.h"
#include "singlePlayerButton.h"
#include "mapEditorButton.h"
#include "optionsButton.h"
#include "exitButton.h"
#include "tabButton.h"
#include "okButton.h"
#include "defaultsButton.h"
#include "backButton.h"
#include "newMapButton.h"
#include "loadMapButton.h"
#include "exportButton.h"
#include "mapListbox.h"
#include "skyboxTextureListbox.h"
#include "landTextureListbox.h"
#include "gameObjectListbox.h"
#include "playButton.h"
#include "inGameAppState.h"
#include "mainMenuButton.h"
#include "buildButton.h"
#include "trainButton.h"
#include "statsButton.h"
#include "researchButton.h"
#include "tradeButton.h"
#include "activeStateButton.h"
#include "playerTradeButton.h"
#include "tradingScreenButton.h"
#include "offerButton.h"
#include "resourceAmmountButton.h"
#include "orderButton.h"
#include "stateToggleButton.h"
#include "minimapButton.h"
#include "activeStateBackButton.h"

namespace battleship{
	using namespace std;
	using namespace gameBase;
	using namespace vb01;
	using namespace vb01Gui;

	static ConcreteGuiManager *concreteGuiManager = nullptr;

	ConcreteGuiManager::ConcreteGuiManager(){
		string assetPath = GameManager::getSingleton()->getPath();
		texBasePath = assetPath + "Textures/";
		fontBasePath = assetPath + "Fonts/";
	}

	ConcreteGuiManager* ConcreteGuiManager::getSingleton(){
		if(!concreteGuiManager)
			concreteGuiManager = new ConcreteGuiManager();

		return concreteGuiManager;
	}

	Tooltip* ConcreteGuiManager::parseTooltip(sol::table &tooltipTbl, Vector3 guiPos){
		sol::table offsetTbl = tooltipTbl["offset"], sizeTbl = tooltipTbl["size"];
		Vector3 offset = Vector3(offsetTbl["x"], offsetTbl["y"], offsetTbl["z"]);
		Vector2 size = Vector2(sizeTbl["x"], sizeTbl["y"]);

		sol::table lineDataTbl = tooltipTbl["lines"];
		int numLines = lineDataTbl.size();
		vector<Tooltip::LineData> lines;

		for(int i = 0; i < numLines; i++){
			vector<pair<int, string>> iconsData;
			sol::optional<sol::table> iconsTblOpt = lineDataTbl[i + 1]["icons"];

			if(iconsTblOpt != sol::nullopt){
				sol::table iconsTbl = lineDataTbl[i + 1]["icons"];
				int numIcons = iconsTbl.size();

				for(int j = 0; j < numIcons; j++)
					iconsData.push_back(make_pair(iconsTbl[j + 1]["charId"], iconsTbl[j + 1]["path"]));
			}

			sol::table entryTbl = lineDataTbl[i + 1]["entry"];
			wstring entry = (wstring)entryTbl["text"];
			Vector4 color = Vector4::VEC_IJKL;
			sol::optional<sol::table> colorTblOpt = entryTbl["color"];

			if(colorTblOpt != sol::nullopt)
				color = Vector4(entryTbl["color"]["r"], entryTbl["color"]["g"], entryTbl["color"]["b"], entryTbl["color"]["a"]);

			int height = 20;
			float scale = .2;
			Vector3 textOffset = Vector3(0, -3, .21);

			lines.push_back(Tooltip::LineData(entry, color, iconsData, height, textOffset, scale));
		}

		string fontPath = GameManager::getSingleton()->getPath() + "Fonts/batang.ttf";
		Tooltip *tooltip = new Tooltip(guiPos + offset, size, lines, fontPath);

		sol::optional<sol::table> bgColorTblOpt = tooltipTbl["backgroundColor"];

		if(bgColorTblOpt != sol::nullopt){
			sol::table bgColorTbl = tooltipTbl["backgroundColor"];
			Vector4 bgColor = Vector4(bgColorTbl["x"], bgColorTbl["y"], bgColorTbl["z"], bgColorTbl["w"]);
			tooltip->getBackground()->getMaterial()->setVec4Uniform("diffuseColor", bgColor);
		}

		return tooltip;
	}

	//TODO refactor player difficulty and faction listbox selection
	//TODO remove hardcoded font path values
	//TODO use configurable map path values 
	Button* ConcreteGuiManager::parseButton(int guiId){
		sol::state_view SOL_LUA_STATE = generateView();
		sol::table guiTable = SOL_LUA_STATE["gui"][guiId + 1];

		sol::table posTable = guiTable["pos"];
		Vector3 pos = Vector3(posTable["x"], posTable["y"], posTable["z"]);

		sol::table sizeTable = guiTable["size"];
		Vector2 size = Vector2(sizeTable["x"], sizeTable["y"]);

		//TODO factor out repetetive optional lua key checks
		string name = "", nk = "name";
		sol::optional<string> nameOpt = guiTable[nk];

		if(nameOpt != sol::nullopt) name = guiTable[nk];

		ButtonType type = (ButtonType)guiTable["buttonType"];

		string imagePath = "", ipk = "imagePath";
		sol::optional<string> pathOpt = guiTable[ipk];
		bool texturingEnabled = false;

		if(pathOpt != sol::nullopt){
			imagePath = texBasePath;
			imagePath += guiTable[ipk];
			bool texturingEnabled = true;
		}

		Tooltip *tooltip = nullptr;
		sol::optional<sol::table> tooltipTblOpt = guiTable["tooltip"];

		if(tooltipTblOpt != sol::nullopt){
			sol::table tbl = guiTable["tooltip"];
			tooltip = parseTooltip(tbl, pos);
		}

		PfButtonBase *button = nullptr;
		string guiScreen = "";

		switch(type){
			case SINGLE_PLAYER:
				button = new SinglePlayerButton(pos, size, name);
				break;
			case EDITOR:
				button = new MapEditorButton(pos, size);
				break;
			case OPTIONS:
				button = new OptionsButton(pos, size, name, true);
				break;
			case EXIT:
				button = new ExitButton(pos, size);
				break;
			case OK:
				button = new OkButton(pos, size, name);
				break;
			case DEFAULTS:
				button = new DefaultsButton(pos, size, name);
				break;
			case BACK: {
				string screen = guiTable["screen"];
				button = new BackButton(pos, size, name, screen);
				break;
			}
			case CONTROLS_TAB:
			case MOUSE_TAB:
			case VIDEO_TAB:
			case AUDIO_TAB:
			case MULTIPLAYER_TAB: {
				string screens[]{
					"controlsTab.lua",
					"mouseTab.lua",
					"videoTab.lua",
					"audioTab.lua",
					"multiplayerTab.lua"
				};
				int diff = ((int)type - (int)CONTROLS_TAB);
				button = new TabButton(pos, size, name, screens[diff]);
				break;
			}
			case NEW_MAP:
				button = new NewMapButton(pos, size);
				break;
			case NEW_MAP_OK:{
				int numTextboxes = guiTable["numDependencies"];
				vector<Textbox*> t;

				for(int i = 0; i < numTextboxes; i++){
					int tid = guiTable["dependencies"][i + 1]["id"];
					t.push_back((Textbox*)guiElements[tid].second);
				}

				button = new NewMapButton::OkButton(pos, size, t[0], t[1], t[2]);
				break;
			}
			case LOAD_MAP:
				button = new LoadMapButton(pos, size);
				break;
			case LOAD_MAP_OK:{
				int lid = guiTable["dependencies"][1]["id"];
				button = new LoadMapButton::OkButton(pos, size, (Listbox*)guiElements[lid].second);
				break;
			}
			case EXPORT:
				button = new ExportButton(pos, size);
				break;
			case PLAY:{
				int mid = guiTable["dependencies"][1]["id"];
				Listbox *mapListbox = (MapListbox*)guiElements[mid].second;
				button = new PlayButton(mapListbox, pos, size, name, true);
				break;
			}
			case RESUME:
				button = new InGameAppState::ResumeButton(pos, size);
				break;
			case CONSOLE_SCREEN:
				button = new InGameAppState::ConsoleButton(pos, size);
				break;
			case MAIN_MENU:
				button = new MainMenuButton(pos, size, name);
				break;
			case CONSOLE_COMMAND_OK:{
				int lid = guiTable["dependencies"][1]["id"];
				Listbox *listbox = (Listbox*)guiElements[lid].second;

				int tid = guiTable["dependencies"][2]["id"];
				Textbox *textbox = (Textbox*)guiElements[tid].second;

				button = new InGameAppState::ConsoleButton::ConsoleCommandEntryButton(textbox, listbox, pos, size, name);
				break;
			}
			case BUILD:
				button = new BuildButton(pos, size, name, (int)guiTable["trigger"], imagePath, (int)guiTable["slotId"]);
				break;
			case TRAIN:
				button = new TrainButton(pos, size, name, (int)guiTable["trigger"], imagePath, (int)guiTable["slotId"]);
				break;
			case STATISTICS:
				button = new StatsButton(pos, size, name, (int)guiTable["trigger"], imagePath);
				break;
			case RESEARCH:
				button = new ResearchButton(pos, size, name, (int)guiTable["trigger"], imagePath, (int)guiTable["techId"]);
				break;
			case BUY_REFINEDS:
				button = new TradeButton(pos, size, name, (int)guiTable["trigger"], imagePath, TradeButton::Type::BUY_REFINEDS, (int)guiTable["amount"]);
				break;
			case SELL_REFINEDS:
				button = new TradeButton(pos, size, name, (int)guiTable["trigger"], imagePath, TradeButton::Type::SELL_REFINEDS, (int)guiTable["amount"]);
				break;
			case BUY_RESEARCH:
				button = new TradeButton(pos, size, name, (int)guiTable["trigger"], imagePath, TradeButton::Type::BUY_RESEARCH, (int)guiTable["amount"]);
				break;
			case SELL_RESEARCH:
				button = new TradeButton(pos, size, name, (int)guiTable["trigger"], imagePath, TradeButton::Type::SELL_RESEARCH, (int)guiTable["amount"]);
				break;
			case ACTIVE_STATE_BUTTON:
				button = new ActiveStateButton(pos, size, guiTable["guiScreen"], name, fontBasePath + "batang.ttf", (int)guiTable["trigger"], imagePath);
				break;
			case ACTIVE_STATE_BACK:
				button = new ActiveStateBackButton(pos, size, name);
				break;
			case PLAYER_TRADE:
				guiScreen = guiTable["guiScreen"];
				button = new PlayerTradeButton(pos, size, guiScreen, name, (int)guiTable["trigger"], imagePath);
				break;
			case TRADING_SCREEN:{
				int lid = guiTable["dependencies"][1]["id"];
				Listbox *listbox = (Listbox*)guiElements[lid].second;
				guiScreen = guiTable["guiScreen"];

				button = new TradingScreenButton(pos, size, listbox, (int)SOL_LUA_STATE["playerId"], guiScreen, name, (int)guiTable["trigger"], imagePath);
				break;
			}
			case TRADE_OFFER:
				button = new OfferButton(pos, size, (int)SOL_LUA_STATE["playerId"], name, (int)guiTable["trigger"], imagePath);
				break;
			case RESOURCE_AMMOUNT:
				button = new ResourceAmmountButton(pos, size, name, (int)guiTable["ammount"], (int)guiTable["trigger"], imagePath);
				break;
			case ORDER:
				button = new OrderButton(pos, size, name, (int)guiTable["trigger"], imagePath, (int)guiTable["orderType"]);
				break;
			case UNIT_STATE:
				button = new StateToggleButton(pos, size, name, (int)guiTable["trigger"], imagePath);
				break;
			case MINIMAP:{
				string minimapPath = GameManager::getSingleton()->getPath() + "Models/Maps/" + Map::getSingleton()->getMapName() + "/minimap.jpg";
				button = new MinimapButton(pos, size, minimapPath);
				break;
			}
		}

		sol::optional<sol::table> baseColorTblOpt = guiTable["baseColor"], hoveredColorTblOpt = guiTable["hoveredOnColor"];

		if(baseColorTblOpt != sol::nullopt){
			sol::table baseColorTbl = guiTable["baseColor"];
			Vector4 color = Vector4(baseColorTbl["x"], baseColorTbl["y"], baseColorTbl["z"], baseColorTbl["w"]);
			button->setBaseColor(color);
			button->setColor(color);
		}

		if(hoveredColorTblOpt != sol::nullopt){
			sol::table hoveredColorTbl = guiTable["hoveredOnColor"];
			button->setHoveredOnColor(Vector4(hoveredColorTbl["x"], hoveredColorTbl["y"], hoveredColorTbl["z"], hoveredColorTbl["w"]));
			button->setEnabledHoverdOn(true);
		}

		button->setTooltip(tooltip);

		int typeArr[2]{(int)GuiElementType::BUTTON, (int)type};
	 	guiElements.push_back(make_pair(typeArr, (void*)button));

		return button;
	}

	Listbox* ConcreteGuiManager::parseGameObjectListbox(){
		return nullptr;
	}

	Listbox* ConcreteGuiManager::parseListbox(int guiId){
		sol::state_view SOL_LUA_STATE = generateView();
		sol::table guiTable = SOL_LUA_STATE["gui"][guiId + 1];

		string posTable = "pos";
		Vector3 pos = Vector3(guiTable[posTable]["x"], guiTable[posTable]["y"], guiTable[posTable]["z"]);

		string sizeTable = "size";
		Vector2 size = Vector2(guiTable[sizeTable]["x"], guiTable[sizeTable]["y"]);

		int numMaxDisplay = guiTable["numMaxDisplay"];
		ListboxType listboxType = (ListboxType)guiTable["listboxType"];

		int maxDisplay, numLines;
		bool closable;
		vector<string> lines;
		sol::optional<sol::table> linesOpt = guiTable["lines"];

		if(linesOpt != sol::nullopt){
			sol::table linesTbl = guiTable["lines"];

			for(int i = 0; i < linesTbl.size(); i++)
		   		lines.push_back(guiTable["lines"][i + 1]);
		}

		Listbox *listbox = nullptr;
		string fontPath = fontBasePath + "batang.ttf";
		sol::optional<string> nameOpt = guiTable["name"];
		string name = "";

		if(nameOpt != sol::nullopt) name = guiTable["name"];

		switch(listboxType){
			case CONTROLS:{
				numLines = 6;
				closable = false;

				for(int i = 0; i < numLines; i++)
					lines.push_back(to_string(i));

				maxDisplay = (numLines > numMaxDisplay ? numMaxDisplay : numLines);
				listbox = new Listbox(pos, size, lines, maxDisplay, fontPath, closable);
				
				break;
			}
			case RESOLUTION:{
				numLines = guiTable["numLines"];

				for(int i = 0; i < numLines; i++)
					lines.push_back(guiTable["lines"][i + 1]);

				closable = true;
				maxDisplay = (numLines > numMaxDisplay ? numMaxDisplay : numLines);

				listbox = new Listbox(pos, size, lines, maxDisplay, fontPath, closable);

				break;
			}
			case MAPS:{
				lines = readDir(GameManager::getSingleton()->getPath() + "Models/Maps/", true);
				numLines = lines.size();
				closable = true;
				maxDisplay = (numLines > numMaxDisplay ? numMaxDisplay : numLines);
				bool addPlayers = guiTable["addPlayerGui"];

				listbox = new MapListbox(pos, size, lines, maxDisplay, addPlayers, fontPath, closable);
				break;
			}
			case VEHICLES:
			case STRUCTURES:
			case RESOURCE_DEPOSITS:{
				bool resources = (listboxType == RESOURCE_DEPOSITS);
				sol::table gameObjTable = SOL_LUA_STATE[resources ? "resources" : "units"];
				int numGameObjs = gameObjTable.size();
				std::vector<int> gameObjIds;
				
				for(int i = 0; i < numGameObjs; i++){
					bool canAdd = true;

					if(!resources){
						bool vehicles = (listboxType == VEHICLES);
						bool v = gameObjTable[i + 1]["isVehicle"];
						canAdd = (v == vehicles);
					}

					if(canAdd){
						lines.push_back(gameObjTable[i + 1]["name"]);
						gameObjIds.push_back(i);
					}
				}
				
				numLines = lines.size();
				closable = true;
				maxDisplay = (numLines > numMaxDisplay ? numMaxDisplay : numLines);
				
				listbox = new GameObjectListbox(!resources, pos, size, lines, gameObjIds, maxDisplay, fontPath);
				break;
			}
			case SKYBOX_TEXTURES:
				lines = readDir(texBasePath + "Skyboxes", true);
				numLines = lines.size();
				closable = true;
				maxDisplay = (numLines > numMaxDisplay ? numMaxDisplay : numLines);

				listbox = new SkyboxTextureListbox(pos, size, lines, maxDisplay, fontPath);
				break;
			case LAND_TEXTURES:
				lines = readDir(texBasePath + "Landmass", false);
				numLines = lines.size();
				closable = true;
				maxDisplay = (numLines > numMaxDisplay ? numMaxDisplay : numLines);

				listbox = new LandTextureListbox(pos, size, lines, maxDisplay, fontPath);
				break;
			case CPU_DIFFICULTIES:
			case FACTIONS:
			case TEAMS:
				numLines = lines.size();
				closable = true;
				maxDisplay = (numLines > numMaxDisplay ? numMaxDisplay : numLines);

				listbox = new Listbox(pos, size, lines, maxDisplay, fontPath);
				break;
			case COLORS:{
				numLines = lines.size();
				closable = true;
				maxDisplay = (numLines > numMaxDisplay ? numMaxDisplay : numLines);
				SOL_LUA_STATE.script("colorCodes = generateColorTable(colors, 'code')");
				sol::table colorCodesTbl = SOL_LUA_STATE["colorCodes"];

				listbox = new Listbox(pos, size, lines, maxDisplay, fontPath);

				for(int i = 0; i < numLines; i++){
					Vector4 col = Vector4(colorCodesTbl[i + 1]["r"], colorCodesTbl[i + 1]["g"], colorCodesTbl[i + 1]["b"], 1);
					listbox->getLineText(i)->getMaterial()->setVec4Uniform("diffuseColor", col);
				}

				break;
			}
			case CONSOLE:{
				for(int i = 0; i < numMaxDisplay; i++)
					lines.push_back("");

				closable = false;
				maxDisplay = (numLines > numMaxDisplay ? numMaxDisplay : numLines);

				listbox = new Listbox(pos, size, lines, maxDisplay, fontPath, closable);
			}
				break;
			case TRADE_OFFERS:
				closable = true;
				listbox = new Listbox(pos, size, lines, numMaxDisplay, fontPath, closable);
				break;
		}

		int typeArr[2]{(int)GuiElementType::LISTBOX, (int)listboxType};
	 	guiElements.push_back(make_pair(typeArr, (void*)listbox));

		listbox->setName(name);

		return listbox;
	}

	Checkbox* ConcreteGuiManager::parseCheckbox(int guiId){
		sol::state_view SOL_LUA_STATE = generateView();
		sol::table guiTable = SOL_LUA_STATE["gui"][guiId + 1];

		string posTable = "pos";
		Vector3 pos = Vector3(guiTable[posTable]["x"], guiTable[posTable]["y"], guiTable[posTable]["z"]);
		Checkbox *checkbox = new Checkbox(pos, fontBasePath + "batang.ttf");

		int typeArr[2]{(int)GuiElementType::CHECKBOX, -1};
	 	guiElements.push_back(make_pair(typeArr, (void*)checkbox));

		return checkbox;
	}

	Slider* ConcreteGuiManager::parseSlider(int guiId){
		sol::state_view SOL_LUA_STATE = generateView();
		sol::table guiTable = SOL_LUA_STATE["gui"][guiId + 1];

		string posTable = "pos", sizeTable = "size";
		Vector3 pos = Vector3(guiTable[posTable]["x"], guiTable[posTable]["y"], guiTable[posTable]["z"]);
		Vector2 size = Vector2(guiTable[sizeTable]["x"], guiTable[sizeTable]["y"]);

		Slider *slider = new Slider(pos, size, guiTable["minValue"], guiTable["maxValue"]);

		int typeArr[2]{(int)GuiElementType::SLIDER, -1};
	 	guiElements.push_back(make_pair(typeArr, (void*)slider));

		return slider;
	}

	Textbox* ConcreteGuiManager::parseTextbox(int guiId){
		sol::state_view SOL_LUA_STATE = generateView();
		sol::table guiTable = SOL_LUA_STATE["gui"][guiId + 1];

		string posTable = "pos", sizeTable = "size";
		Vector3 pos = Vector3(guiTable[posTable]["x"], guiTable[posTable]["y"], guiTable[posTable]["z"]);
		Vector2 size = Vector2(guiTable[sizeTable]["x"], guiTable[sizeTable]["y"]);
		Textbox *textbox = new Textbox(pos, size, fontBasePath + "batang.ttf");

		int typeArr[2]{(int)GuiElementType::TEXTBOX, -1};
	 	guiElements.push_back(make_pair(typeArr, (void*)textbox));

		return textbox;
	}

	//TODO factor out checking for optional lua values
	Node* ConcreteGuiManager::parseGuiRectangle(int guiId){
		sol::state_view SOL_LUA_STATE = generateView();
		sol::table guiTable = SOL_LUA_STATE["gui"][guiId + 1];

		Root *root = Root::getSingleton();
		Material *mat = new Material(root->getLibPath() + "gui");
		mat->setTransparent(true);

		bool texturingEnabled = false;
		string imagePath = "", ipk = "imagePath";
		sol::optional<string> pathOpt = guiTable[ipk];

		if(pathOpt != sol::nullopt){
			imagePath = guiTable[ipk];
			texturingEnabled = true;
		}

		string name = "", nk = "name";
		sol::optional<string> nameOpt = guiTable[nk];

		if(nameOpt != sol::nullopt) name = guiTable[nk];

		mat->addBoolUniform("texturingEnabled", texturingEnabled);

		if(texturingEnabled){
			string p[]{texBasePath + imagePath};
			Texture *tex = new Texture(p, 1, false);
			mat->addTexUniform("diffuseMap", tex, false);
		}
		else{
			sol::table colorTable = guiTable["color"];
			mat->addVec4Uniform("diffuseColor", Vector4(colorTable["x"], colorTable["y"], colorTable["z"], colorTable["w"]));
		}

		sol::table sizeTable = guiTable["size"];
		Quad *quad = new Quad(Vector3(sizeTable["x"], sizeTable["y"], 1), false);
		quad->setMaterial(mat);

		sol::table posTable = guiTable["pos"];
		Node *guiRectangle = new Node(Vector3(posTable["x"], posTable["y"], posTable["z"]), Quaternion::QUAT_W, Vector3::VEC_IJK, name);
		guiRectangle->attachMesh(quad);
		root->getGuiNode()->attachChild(guiRectangle);

		int typeArr[2]{(int)GuiElementType::GUI_RECTANGLE, -1};
	 	guiElements.push_back(make_pair(typeArr, (void*)guiRectangle));

		return guiRectangle;
	}

	//TODO distinguish between floats and vector-like tables for scale
	Text* ConcreteGuiManager::parseText(int guiId){
		sol::state_view SOL_LUA_STATE = generateView();
		sol::table guiTable = SOL_LUA_STATE["gui"][guiId + 1];
		sol::table posTable = guiTable["pos"];

		Root *root = Root::getSingleton();

		Material *mat = new Material(root->getLibPath() + "text");
		mat->addBoolUniform("texturingEnabled", false);
		sol::table colorTable = guiTable["color"];
		mat->addVec4Uniform("diffuseColor", Vector4(colorTable["x"], colorTable["y"], colorTable["z"], colorTable["w"]));

		string font = guiTable["font"];
		wstring entry = guiTable["text"];
		Text *text = new Text(fontBasePath + font, entry, guiTable["fontFirstChar"], guiTable["fontLastChar"]);
		text->setMaterial(mat);

		Vector3 pos = Vector3(posTable["x"], posTable["y"], posTable["z"]);

		SOL_LUA_STATE.script("tp = type(gui[" + to_string(guiId + 1) + "].scale)");
		string st = SOL_LUA_STATE["tp"];
		Vector3 scale;

		if(st == "table"){
			sol::table scaleTable = guiTable["scale"];
			scale = Vector3(scaleTable["x"], scaleTable["y"], 1);
		}
		else if(st == "number"){
			float sc = guiTable["scale"];
			scale = Vector3(sc, sc, 1);
		}

		Node *node = new Node(pos, Quaternion::QUAT_W, scale, guiTable["name"]);
		node->addText(text);
		root->getGuiNode()->attachChild(node);

		int typeArr[2]{(int)GuiElementType::TEXT, -1};
	 	guiElements.push_back(make_pair(typeArr, (void*)text));

		return text;
	}

	void ConcreteGuiManager::parseMusic(){
		sol::state_view SOL_STATE_VIEW = generateView();
		sol::optional<sol::table> musicTblOpt = SOL_STATE_VIEW["music"];

		if(musicTblOpt == sol::nullopt) return;

		sol::table musicTbl = SOL_STATE_VIEW["music"], tracksTbl = musicTbl["tracks"];
		SoundManager *sm = SoundManager::getSingleton();

		if(tracksTbl.size() == 0){
			sm->clearPlaylist();
			return;
		}

		bool loop = musicTbl["loop"], shuffle = musicTbl["shuffle"];
		int delay = musicTbl["delay"].get_or(0);
		int numTracks = tracksTbl.size();

		vector<string> trackPaths;

		for(int i = 0; i < numTracks; i++){
			string track = tracksTbl[i + 1];
			trackPaths.push_back(GameManager::getSingleton()->getPath() + "Sounds/Music/" + track);
		}

		sm->play(trackPaths, 100, delay, loop, shuffle);
	}

	void ConcreteGuiManager::readLuaScreenScript(
			string script,
			vector<Button*> buttonExceptions,
			vector<Listbox*> listboxExceptions,
			vector<Checkbox*> checkboxExceptions,
			vector<Slider*> sliderExceptions,
			vector<Textbox*> textboxExceptions,
			vector<Node*> guiRectboxExceptions,
			vector<Text*> textExceptions,
			string luaCode
		){
		removeAllGuiElements(buttonExceptions, listboxExceptions, checkboxExceptions, sliderExceptions, textboxExceptions, guiRectboxExceptions, textExceptions);
		parseLuaScript(script, luaCode);
	}

	void ConcreteGuiManager::readLuaScreenScriptDel(
			string script,
			vector<Button*> buttons,
			vector<Listbox*> listboxs,
			vector<Checkbox*> checkboxs,
			vector<Slider*> sliders,
			vector<Textbox*> textboxs,
			vector<Node*> guiRectboxs,
			vector<Text*> texts
		){
		for(Button *b : buttons) removeButton(b);
		for(Listbox *l : listboxs) removeListbox(l);
		for(Checkbox *c : checkboxs) removeCheckbox(c);
		for(Slider *s : sliders) removeSlider(s);
		for(Textbox *t : textboxs) removeTextbox(t);
		for(Node *r : guiRectboxs) removeGuiRectangle(r);
		for(Text *t : texts) removeText(t);

		parseLuaScript(script);
	}

	void ConcreteGuiManager::parseLuaScript(string script, string luaCode){
		guiElements.clear();

		string basePath = GameManager::getSingleton()->getPath() + "Scripts/Gui/";
		sol::state_view SOL_LUA_VIEW = generateView();
		SOL_LUA_VIEW.script("music = nil");
		SOL_LUA_VIEW.script_file(basePath + script);

		if(luaCode != "") SOL_LUA_VIEW.script(luaCode);

		SOL_LUA_VIEW.script("numGui = #gui");
		int numGuiElements = SOL_LUA_VIEW["numGui"];

		for(int i = 0; i < numGuiElements; i++){
			int guiTypeId = SOL_LUA_VIEW["gui"][i + 1]["guiType"];

			switch((GuiElementType)guiTypeId){
				case BUTTON:
					addButton(parseButton(i));
					break;
				case LISTBOX:
					addListbox(parseListbox(i));
					break;
				case CHECKBOX:
					addCheckbox(parseCheckbox(i));
					break;
				case SLIDER:
					addSlider(parseSlider(i));
					break;
				case TEXTBOX:
					addTextbox(parseTextbox(i));
					break;
				case GUI_RECTANGLE:
					addGuiRectangle(parseGuiRectangle(i));
					break;
				case TEXT:
					addText(parseText(i));
					break;
			}
		}

		parseMusic();
	}
}
