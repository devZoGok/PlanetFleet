#include "optionsButton.h"
#include "gameManager.h"
#include "concreteGuiManager.h"

using namespace vb01Gui;
using namespace vb01;
using namespace std;

namespace battleship{
    OptionsButton::OptionsButton(Vector3 pos, Vector2 size, string name, bool separate) : Button(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", -1, separate) {}

    void OptionsButton::onClick() {
		ConcreteGuiManager::getSingleton()->readLuaScreenScript("options.lua");
	}
}
