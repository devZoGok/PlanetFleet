#pragma once
#ifndef GUI_APP_STATE_H
#define GUI_APP_STATE_H

#include <button.h>
#include <listbox.h>
#include <checkbox.h>
#include <textbox.h>
#include <slider.h>

#include <vector>

#include <abstractAppState.h>

#include "binds.h"
#include "tooltip.h"

namespace battleship{
    class GuiAppState : public gameBase::AbstractAppState {
    public:
        GuiAppState();
        ~GuiAppState();
        void onAttached();
        void onDettached();
        void update();
        inline bool isLeftMousePressed(){return leftMousePressed;}
    private:
        virtual void onAction(int, bool);
        virtual void onAnalog(int, float);
        virtual void onRawKeyPress(int);
		virtual void onRawCharPress(vb01::u32);
        virtual void onRawMousePress(int);
        virtual void onRawMouseWheelScroll(bool);
        vb01Gui::Textbox* getOpenTextbox();
        vb01Gui::Listbox* getOpenListbox();
        
        bool leftMousePressed = false, backspacePressed = false;
    protected:
    };
}

#endif
