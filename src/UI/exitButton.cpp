#include "exitButton.h"
#include "gameManager.h"

using namespace std;
using namespace vb01;

namespace battleship{
    ExitButton::ExitButton(Vector3 pos, Vector2 size) : Button(pos, size, "Exit", GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", -1, true) {}

    void ExitButton::onClick() {
		GameManager::getSingleton()->setRunning(false);
    }
}
