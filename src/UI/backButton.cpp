#include "backButton.h"
#include "gameManager.h"
#include "concreteGuiManager.h"

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;

    BackButton::BackButton(Vector3 pos, Vector2 size, string name, string scr) :
		Button(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", -1, true),
		screen(scr){}

    void BackButton::onClick() {
		ConcreteGuiManager::getSingleton()->readLuaScreenScript(screen);
	}
}
