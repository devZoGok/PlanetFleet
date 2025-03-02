#include "mapListbox.h"
#include "map.h"
#include "concreteGuiManager.h"
#include "gameManager.h"

#include <solUtil.h>

#include <button.h>
#include <checkbox.h>
#include <slider.h>
#include <textbox.h>
#include <node.h>
#include <text.h>

namespace battleship{
	using namespace vb01;
	using namespace vb01Gui;
	using namespace std;
	using namespace gameBase;

	MapListbox::MapListbox(Vector3 pos, Vector2 size, std::vector<string> &lines, int maxDisplay, bool adg, string fontPath, bool closeable) : 
		Listbox(pos, size, lines, maxDisplay, fontPath, closeable), 
		addPlayerGui(adg) {
			openUp();
			close();
	}

	void MapListbox::onClose(){
		if(addPlayerGui){
			ConcreteGuiManager *guiManager = ConcreteGuiManager::getSingleton();

			for(Listbox *listbox : cpuPlayerListboxes)
				guiManager->removeListbox(listbox);

			cpuPlayerListboxes.clear();

			string mapName = wstringToString(getContents()[getSelectedOption()]);
			int numSpawnPoints = Map::getSingleton()->getNumMapSpawnPoints(mapName);
			sol::state_view SOL_LUA_VIEW = generateView();

			for(int i = 0; i < numSpawnPoints - 1; i++){
				int prevNumListboxes = guiManager->getListboxes().size();

				SOL_LUA_VIEW.script("lineId = " + to_string(i));
		   		guiManager->parseLuaScript("playerSelection.lua");

				std::vector<Listbox*> listboxes = guiManager->getListboxes();
				int diffNumListboxes = guiManager->getListboxes().size() - prevNumListboxes;

				for(int j = 0; j < diffNumListboxes; j++)
					cpuPlayerListboxes.push_back(listboxes[listboxes.size() - 1 - j]);
			}
		}
	}
}
