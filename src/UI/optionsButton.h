#pragma once
#ifndef OPTIONS_BUTTON_H
#define OPTIONS_BUTTON_H

#include <button.h>

namespace battleship {
    class OptionsButton : public vb01Gui::Button {
    public:
        OptionsButton(vb01::Vector3, vb01::Vector2, std::string, bool);
        virtual void onClick();
    };
}

#endif
