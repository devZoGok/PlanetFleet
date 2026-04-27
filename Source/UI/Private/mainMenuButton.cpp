#include "mainMenuButton.h"
#include "gameManager.h"
#include "concreteGuiManager.h"

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;

	MainMenuButton::MainMenuButton(Vector3 pos, Vector2 size, string name) : Button(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", -1, true){}

	void MainMenuButton::onClick(){
	}
}
