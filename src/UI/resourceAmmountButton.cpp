#include "resourceAmmountButton.h"
#include "concreteGuiManager.h"
#include "gameManager.h"

#include <textbox.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;

	ResourceAmmountButton::ResourceAmmountButton(Vector3 pos, Vector2 size, string name, int amm, int trigger, string imagePath) : Button(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", trigger, true, imagePath), ammount(amm) {
		ConcreteGuiManager *guiManager = ConcreteGuiManager::getSingleton();
		int numTextboxes = guiManager->getTextboxes().size();
		textbox = guiManager->getTextboxes()[numTextboxes - 1];
	}

	void ResourceAmmountButton::onClick(){
		wstring text = textbox->getText();
		int amm = (text != L"" ? stoi(text) : 0) + ammount;

		if(minAmmount <= amm && amm <= maxAmmount)
			textbox->setEntry(to_wstring(amm));
	}
}
