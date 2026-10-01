#include <cmath>
#include <iostream>

#include <checkbox.h>
#include <listbox.h>
#include <slider.h>

#include "optionBinding.h"
#include "gameManager.h"

namespace battleship{
	using namespace std;
	using namespace gameBase;
	using namespace vb01Gui;

	OptionBinding::OptionBinding(GuiElementType t, void *el, sol::table opt) : type(t), guiElement(el), option(opt), path(opt["path"].get<string>()){}

	void OptionBinding::load(){
		sol::state_view SOL_LUA_STATE = generateView();
		sol::protected_function getOption = SOL_LUA_STATE["getOption"];

		switch(type){
			case CHECKBOX:
				((Checkbox*)guiElement)->setChecked(getOption(path).get<bool>());
				break;
			case SLIDER:
				((Slider*)guiElement)->setValue(getOption(path).get<double>());
				break;
			case LISTBOX:{
				sol::optional<sol::table> valuesTblOpt = option["values"];

				if(valuesTblOpt != sol::nullopt){
					sol::table valuesTbl = option["values"];
					sol::protected_function getOptionIndex = SOL_LUA_STATE["getOptionIndex"];
					((Listbox*)guiElement)->setSelectedOption(getOptionIndex(path, valuesTbl).get<int>());
				}

				break;
			}
			default:
				break;
		}
	}

	void OptionBinding::store(){
		sol::protected_function setOption = generateView()["setOption"];

		switch(type){
			case CHECKBOX:
				setOption(path, ((Checkbox*)guiElement)->isChecked());
				break;
			case SLIDER:{
				Slider *slider = (Slider*)guiElement;
				double value = slider->getValue();
				sol::optional<double> stepOpt = option["step"];

				if(stepOpt != sol::nullopt){
					double step = option["step"];
					value = round(value / step) * step;
					slider->setValue(value);
				}

				setOption(path, value);
				break;
			}
			case LISTBOX:{
				sol::optional<sol::table> valuesTblOpt = option["values"];

				if(valuesTblOpt != sol::nullopt){
					sol::object value = option["values"][((Listbox*)guiElement)->getSelectedOption() + 1];

					if(value.valid()) setOption(path, value);
				}

				break;
			}
			default:
				break;
		}
	}

	void OptionBinding::restoreDefault(){
		sol::protected_function restoreDefaultOption = generateView()["restoreDefaultOption"];
		restoreDefaultOption(path);
		load();
	}

	void OptionBinding::save(){
		sol::protected_function saveOptions = generateView()["saveOptions"];
		sol::protected_function_result result = saveOptions();

		if(!result.valid()){
			sol::error err = result;
			cerr << "Failed to save options: " << err.what() << endl;
		}
	}

	//applies the options the running game can take on without a restart
	void OptionBinding::apply(){
		if(GameManager::getSingleton()->applyGraphicsOptions())
			ConcreteGuiManager::getSingleton()->reloadScreen();
	}
}
