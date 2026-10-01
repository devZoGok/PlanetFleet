#include "defaultsButton.h"
#include "gameManager.h"

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;

    DefaultsButton::DefaultsButton(Vector3 pos, Vector2 size, string name, vector<OptionBinding> bindings) :
		PfButtonBase(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", -1, true),
		optionBindings(bindings){}

    void DefaultsButton::onClick() {
		for(OptionBinding &binding : optionBindings)
			binding.restoreDefault();

		OptionBinding::save();
		OptionBinding::apply();
	}
}
