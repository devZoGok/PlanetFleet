#ifndef EXIT_BUTTON_H
#define EXIT_BUTTON_H

#include "pfButtonBase.h"

#include <vector.h>

namespace battleship {
    class ExitButton : public PfButtonBase {
    public:
        ExitButton(vb01::Vector3, vb01::Vector2);
        void onClick();
    };
}

#endif
