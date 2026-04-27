#ifndef EXIT_BUTTON_H
#define EXIT_BUTTON_H

#include <button.h>
#include <vector.h>

namespace battleship {
    class ExitButton : public vb01Gui::Button {
    public:
        ExitButton(vb01::Vector3, vb01::Vector2);
        void onClick();
    };
}

#endif
