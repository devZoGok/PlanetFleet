#include "okButton.h"
#include "gameManager.h"

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace vb01Gui;

    OkButton::OkButton(Vector3 pos, Vector2 size, string name, vector<OptionBinding> bindings) :
		PfButtonBase(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", -1, true),
		optionBindings(bindings){}

    void OkButton::onClick() {
		for(OptionBinding &binding : optionBindings)
			binding.store();

		OptionBinding::save();
		OptionBinding::apply();
	}
}
