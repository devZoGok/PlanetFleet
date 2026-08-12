#include "singlePlayerButton.h"
#include "gameManager.h"
#include "concreteGuiManager.h"

#include <listbox.h>
#include <text.h>

#include <glfw3.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;

	SinglePlayerButton::SinglePlayerButton(Vector3 pos, Vector2 size, string name) : Button(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", GLFW_KEY_S, true) {}
	
	void SinglePlayerButton::onMouseOver(){
		setColor(Vector4(.8, .8, .8, 1));
	}
	
	void SinglePlayerButton::onMouseOff(){
		setColor(Vector4(.6, .6, .6, 1));
	}
	
	void SinglePlayerButton::onClick() {
		ConcreteGuiManager *guiManager = ConcreteGuiManager::getSingleton();
		guiManager->readLuaScreenScript("singlePlayerMenu.lua");

		Listbox *listbox = ConcreteGuiManager::getSingleton()->getListboxes()[0];
		listbox->openUp();
		listbox->close();

		guiManager->getText("_mainPlayer")->setText(GameManager::getSingleton()->getMainPlayerName());
	}
}
