#include "trainButton.h"
#include "activeGameState.h"
#include "buildableUnit.h"
#include "factory.h"

#include <stateManager.h>
#include <solUtil.h>

namespace battleship{
	using namespace std;
	using namespace vb01;
	using namespace gameBase;

	TrainButton::TrainButton(Vector3 pos, Vector2 size, string name, int trigger, string imagePath, int slId) :
		UnitButton(pos, size, name, GameManager::getSingleton()->getPath() + "Fonts/batang.ttf", trigger, imagePath),
		slotId(slId) {}

	void TrainButton::onClick(){
		vector<Unit*> factories = getUnits();

		for(Unit *fac : factories)
			((Factory*)fac)->appendToQueue(slotId);
	}
}
