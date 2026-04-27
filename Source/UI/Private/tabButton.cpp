#include "tabButton.h"
#include "concreteGuiManager.h"
#include "gameManager.h"

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;

    TabButton::TabButton(Vector3 pos, Vector2 size, string name, string screenScript) : Button(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", -1, true) {
		this->screenScript = screenScript;
	}

    void TabButton::onClick() {
		ConcreteGuiManager::getSingleton()->readLuaScreenScript(screenScript);
    }
}
