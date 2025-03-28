#include <cmath>
#include <chrono>
#include <thread>

#include <camera.h>
#include <quad.h>
#include <root.h>
#include <vector.h>
#include <util.h>
#include <assetManager.h>

#include <stateManager.h>
#include <solUtil.h>

#include <glm.hpp>
#include <ext.hpp>

#include <tinydir.h>

#include "util.h"
#include "defConfigs.h"
#include "gameManager.h"
#include "concreteGuiManager.h"
#include "guiAppState.h"
#include "inGameAppState.h"
#include "loadingAppState.h"
#include "mapEditorButton.h"

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;
using namespace glm;
using namespace vb01;
using namespace vb01Gui;

namespace battleship{
	using namespace configData;

	void handleLoadingGui(LoadingAppState *loadState){
		ConcreteGuiManager *guiManager = ConcreteGuiManager::getSingleton();
		guiManager->readLuaScreenScript("loadingScreen.lua");

		sol::state_view SOL_LUA_VIEW = generateView();
		string vfxPrefix = SOL_LUA_VIEW["vfxPrefix"], gameObjPrefix = SOL_LUA_VIEW["gameObjPrefix"];

		AssetManager *assetManager = AssetManager::getSingleton();
		string path = GameManager::getSingleton()->getPath();
		vector<string> assets = vector<string>{path + DEFAULT_TEXTURE};

		assetManager->readDir(path + vfxPrefix, assets, true);
		assetManager->readDir(path + gameObjPrefix, assets, true);

		loadState->setLoadableAssets(assets);

		StateManager *stateManager = GameManager::getSingleton()->getStateManager();
	    stateManager->attachAppState(loadState);
	}

    void makeTitlescreenButtons(GuiAppState *state) {
		/*
        class SpButton : public Button {
        public:
            SpButton(GuiAppState *state, Vector2 pos, Vector2 size, string name, bool separate) : Button(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", GLFW_KEY_S, separate) {
                this->state = state;
            }

			void onMouseOver(){
				setColor(Vector4(.8, .8, .8, 1));
			}

			void onMouseOff(){
				setColor(Vector4(.6, .6, .6, 1));
			}

            void onClick() {
                vector<string> difficulties, factions;

                class PlayButton : public Button {
                public:
                    PlayButton(Listbox **difficulties, Listbox **factions, Listbox *mapListbox, int lengths[2], Vector2 pos, Vector2 size, string name, bool separate) : Button(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", GLFW_KEY_P, separate) {
                        this->state = ((GuiAppState*)GameManager::getSingleton()->getStateManager()->getAppStateByType(AppStateType::GUI_STATE));
                        this->lengths[0] = lengths[0];
                        this->lengths[1] = lengths[1];

                        difficultiesListboxes = difficulties;
                        factionsListboxes = factions;
						this->mapListbox = mapListbox;
                    }

                    void onClick() {
						GameManager *gm = GameManager::getSingleton();
                        std::vector<string> difficulties, factions;

                        for(int i = 0; i < lengths[0]; i++)
                            difficulties.push_back(wstringToString(difficultiesListboxes[i]->getContents()[difficultiesListboxes[i]->getSelectedOption()]));

                        for(int i = 0; i < lengths[1]; i++)
                            factions.push_back(to_string(factionsListboxes[i]->getSelectedOption()));


                        delete[] difficultiesListboxes;
                        delete[] factionsListboxes;

						StateManager *stateManager = gm->getStateManager();
						int selectedMap = mapListbox->getSelectedOption();
						string mapName = wstringToString(mapListbox->getContents()[selectedMap]);
                        stateManager->attachAppState(new InGameAppState(difficulties, factions, mapName));
                        state->removeAllListboxes();
                        state->removeButton("Back");
                        state->removeButton("Play");
                    }
                private:
                    GuiAppState *state;
                    Listbox **difficultiesListboxes, **factionsListboxes, *mapListbox = nullptr;
                    int lengths[2];
                };

                class ReturnButton : public Button {
                public:

                    ReturnButton(GuiAppState *state, Vector2 pos, Vector2 size, string name, bool separate) : Button(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", GLFW_KEY_B, separate) {
                        this->state = state;
                    }

                    void onClick() {
                        state->removeAllCheckboxes();
                        state->removeAllListboxes();
                        state->removeAllSliders();
                        state->removeAllTextboxes();
                        makeTitlescreenButtons(state);
                        state->removeButton("Play");
                        state->removeButton("Back");
                    }
                private:
                    GuiAppState *state;
                };

				GameManager *gm = GameManager::getSingleton();
                Vector2 pos(gm->getWidth() / 8, gm->getHeight() / 8);
                difficulties.push_back("Easy");
                difficulties.push_back("Medium");
                difficulties.push_back("Hard");
                factions.push_back("0");
                factions.push_back("1");


				string font = gm->getPath() + "Fonts/batang.ttf";
                Listbox *cpuDifficulty = new Listbox(Vector2(pos.x, pos.y + 30), Vector2(100, 20), difficulties, 3, font);
                Listbox *cpuFaction = new Listbox(Vector2(pos.x + 110, pos.y + 30), Vector2(100, 20), factions, 2, font);
                Listbox *playerFaction = new Listbox(Vector2(pos.x + 110, pos.y), Vector2(100, 20), factions, 2, font);

				vector<string> folders = readDir(gm->getPath() + "Models/Maps", true);
				int numMinDirs = 3;
				int numShowDirs = folders.size() < numMinDirs ? folders.size() : numMinDirs;
				Listbox *map = new Listbox(Vector2(pos.x + 110, pos.y + 100), Vector2(100, 20), folders, numShowDirs, font);
                Listbox **difficultyListboxes = new Listbox*[1];
                Listbox **factionListboxes = new Listbox*[2];
								
                difficultyListboxes[0] = cpuDifficulty;
                factionListboxes[0] = playerFaction;
                factionListboxes[1] = cpuFaction;

                int lengths[]{1, 2};
                PlayButton *playButton = new PlayButton(difficultyListboxes, factionListboxes, map, lengths, Vector2(50, gm->getHeight() - 150), Vector2(140, 50), "Play", true);
                ReturnButton *returnButton = new ReturnButton(state, Vector2(200, gm->getHeight() - 150), Vector2(140, 50), "Back", true);
                state->addButton(playButton);
                state->addButton(returnButton);
                state->addListbox(cpuDifficulty);
                state->addListbox(cpuFaction);
                state->addListbox(playerFaction);
				state->addListbox(map);
                state->removeButton("Options");
                state->removeButton("Exit");
                state->removeButton("Singleplayer");

            }
        private:
            GuiAppState *state;
        };

        class MainMenuOptionsButton : public OptionsButton {
        public:

            MainMenuOptionsButton(GuiAppState *state, Vector2 pos, Vector2 size, string name, bool separate) : OptionsButton(pos, size, name, separate) {
                this->state = state;
            }

            void onClick() {
                ReturnButton *returnButton = new ReturnButton(state, Vector2(50, GameManager::getSingleton()->getHeight() - 150), Vector2(150, 50), "Back", true);
                state->addButton(returnButton);
                state->removeButton("Singleplayer");
                state->removeButton("Exit");
                OptionsButton::onClick();
            }

			void onMouseOver(){
				setColor(Vector4(.8, .8, .8, 1));
			}

			void onMouseOff(){
				setColor(Vector4(.6, .6, .6, 1));
			}
        private:
            GuiAppState *state;

            class ReturnButton : public Button {
            public:

                ReturnButton(GuiAppState *state, Vector2 pos, Vector2 size, string name, bool separate) : Button(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", -1, separate) {
                    this->state = state;
                }

                void onClick() {
                    state->removeAllCheckboxes();
                    state->removeAllListboxes();
                    state->removeAllSliders();
                    state->removeAllTextboxes();
                    state->removeButton("Controls");
                    state->removeButton("Mouse");
                    state->removeButton("Video");
                    state->removeButton("Audio");
                    state->removeButton("Multiplayer");
										GameManager *gm = GameManager::getSingleton();
                    SpButton *spButton = new SpButton(state, Vector2(gm->getWidth() / 16, gm->getHeight() / 12), Vector2(150, 40), "Singleplayer", true);
                    MainMenuOptionsButton *optionsButton = new MainMenuOptionsButton(state, Vector2(gm->getWidth() / 16, gm->getHeight() / 12 * 2), Vector2(150, 40), "Options", true);
                    state->addButton(optionsButton);
                    ExitButton *exitButton = new ExitButton(Vector2(gm->getWidth() / 16, gm->getHeight() / 12 * 3), Vector2(150, 40));
                    state->addButton(spButton);
                    state->addButton(exitButton);
                    state->removeButton("Back");
                }
            private:
                GuiAppState *state;
            };
        };

		GameManager *gm = GameManager::getSingleton();
		string font = gm->getPath() + "Fonts/batang.ttf";
		int width = gm->getWidth(), height = gm->getHeight();
		Vector2 size = Vector2(150, 40);

		AssetManager::getSingleton()->load(font);

		SpButton *spButton = new SpButton(state, Vector2(width / 16, height / 12), size, "Singleplayer", true);
		MainMenuOptionsButton *optionsButton = new MainMenuOptionsButton(state, Vector2(width / 16, height / 12 * 2), size, "Options", true);
		MapEditorButton *editorButton = new MapEditorButton(Vector2(width / 16, height / 12 * 3), size);
		ExitButton *exitButton = new ExitButton(Vector2(width / 16, height / 12 * 4), size);
		state->addButton(spButton);
		state->addButton(optionsButton);
		state->addButton(editorButton);
		state->addButton(exitButton);
		 */
    }

	vector<string> readDir(string path, bool findFolders){
		tinydir_dir dir;
		tinydir_open_sorted(&dir, path.c_str());
		vector<string> files;
		
		for (int i = 0; i < dir.n_files; i++) {
			tinydir_file file;
			tinydir_readfile_n(&dir, &file, i);

			if (file.is_dir == findFolders && file.name[0] != '.')
				files.push_back(file.name);
		}
		
		tinydir_close(&dir);

		return files;
	}

	Vector3 spaceToScreen3d(Vector3 pos){
		Root *root = Root::getSingleton();
		Camera *cam = root->getCamera();
		Vector3 dir = cam->getDirection(), up = cam->getUp();
		Vector3 camPos = cam->getPosition();
		mat4 view = lookAt(vec3(camPos.x, camPos.y, camPos.z), vec3(camPos.x + dir.x, camPos.y + dir.y, camPos.z + dir.z), vec3(up.x, up.y, up.z));
		
		float fov = cam->getFov(), width = root->getWidth(), height = root->getHeight(), nearPlane = cam->getNearPlane(), farPlane = cam->getFarPlane();
		mat4 proj = perspective(radians(fov), width / height, nearPlane, farPlane);

		vec4 ndcPos = proj * view * vec4(pos.x, pos.y, pos.z, 1);
		ndcPos.x /= ndcPos.w;
		ndcPos.y /= ndcPos.w;
		ndcPos.z /= ndcPos.w;
		return Vector3(0.5 * width * (1 + ndcPos.x), 0.5 * height * (1 - ndcPos.y), ndcPos.z);
	}

	Vector2 spaceToScreen(Vector3 pos){
		Vector3 pos3d = spaceToScreen3d(pos);
		return Vector2(pos3d.x, pos3d.y);
	}

	Vector3 screenToSpace(Vector2 pos){
		GameManager *gm = GameManager::getSingleton();
		float midWidth = .5 * gm->getWidth(), midHeight = .5 * gm->getHeight(), horOffset, vertOffset;
		bool left, up;

		if(pos.x < midWidth){
			left = true;
			horOffset = pos.x / midWidth - 1; 
		}
		else{
			left = false;
			horOffset = (pos.x - midWidth) / midWidth; 
		}

		if(pos.y < midHeight){
			up = true;
			vertOffset = 1 - pos.y / midHeight;
		}
		else{
			up = false;
			vertOffset = -(pos.y - midHeight) / midHeight;
		}

		Camera *cam = Root::getSingleton()->getCamera();
		float camNorm = cam->getNearPlane(); 
		float tg = tan(radians(cam->getFov()) / 2), ar = midWidth / midHeight;
		float camHeight = camNorm * tg, camWidth = camHeight * ar;
		Vector3 posOffset = (cam->getLeft() * camWidth * horOffset + cam->getUp() * camHeight * vertOffset + cam->getDirection() * camNorm);

		return cam->getPosition() + posOffset;
	}
}
