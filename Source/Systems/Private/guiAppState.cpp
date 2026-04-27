#include <sstream>
#include <algorithm>

#include <vector.h>
#include <util.h>

#include "util.h"
#include "defConfigs.h"
#include "gameManager.h"
#include "concreteGuiManager.h"
#include "guiAppState.h"

using namespace gameBase;
using namespace vb01;
using namespace vb01Gui;
using namespace std;

namespace battleship{
	using namespace configData;

    GuiAppState::GuiAppState() : AbstractAppState(
						AppStateType::GUI_STATE,
					 	configData::calcSumBinds(AppStateType::GUI_STATE, true),
					 	configData::calcSumBinds(AppStateType::GUI_STATE, false),
					 	GameManager::getSingleton()->getPath() + scripts[(int)ScriptFiles::OPTIONS]){
    }

    GuiAppState::~GuiAppState() {}

    void GuiAppState::update() {
		ConcreteGuiManager::getSingleton()->update();
    }

    void GuiAppState::onAttached() {
        AbstractAppState::onAttached();
    }

    void GuiAppState::onDettached() {
        AbstractAppState::onDettached();
    }

    void GuiAppState::onAction(int bind, bool isPressed) {
		ConcreteGuiManager *guiManager = ConcreteGuiManager::getSingleton();

        switch((Bind)bind){
			case Bind::LEFT_CLICK:
				leftMousePressed = isPressed;

            	if(isPressed)
					guiManager->updateGui();
							
            	break;
			case Bind::SCROLLING_UP:{
				Listbox *l = getOpenListbox();
				if(l) l->scrollUp();
            	break;
			}
			case Bind::SCROLLING_DOWN:{
				Listbox *l = getOpenListbox();
				if(l) l->scrollDown();
            	break;
			}
			case Bind::DELETE_CHAR:{
				Textbox *t = getOpenTextbox();
				if(t)t->setDeleteCharacters(isPressed);
				break;
			}
        }
    }

    void GuiAppState::onRawKeyPress(int ch){
		for(Button *b : ConcreteGuiManager::getSingleton()->getButtons())
			if(b->getTrigger() == ch){
				b->onClick();
				break;
			}
    }

	void GuiAppState::onRawCharPress(u32 codepoint){
		Textbox *currentTextbox = getOpenTextbox();

		if(currentTextbox)
			currentTextbox->type(codepoint);
	}
    
    void GuiAppState::onRawMousePress(int trigger){
    }
    
    Textbox* GuiAppState::getOpenTextbox() {
		vector<Textbox*> textboxes = ConcreteGuiManager::getSingleton()->getTextboxes();

        for (int i = 0; i < textboxes.size(); i++)
            if (textboxes[i]->isEnabled())
                return textboxes[i];

		return nullptr;
    }

    Listbox* GuiAppState::getOpenListbox() {
		vector<Listbox*> listboxes = ConcreteGuiManager::getSingleton()->getListboxes();

        for (int i = 0; i < listboxes.size(); i++)
            if (listboxes[i]->isOpen())
				return listboxes[i];

		return nullptr;
    }

    void GuiAppState::onAnalog(int bind, float strength) {}

	void GuiAppState::onRawMouseWheelScroll(bool up){
		Listbox *openListbox = getOpenListbox();

		if(!openListbox) return;

		if(up) openListbox->scrollUp();
		else openListbox->scrollDown();
	}
}
