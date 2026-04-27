#include "iceSheet.h"
#include "player.h"
#include "game.h"

namespace battleship{
	using namespace vb01;
	using namespace std;

	IceSheet::IceSheet(Player *player, int id, Vector3 pos, Quaternion rot, int bldSt) : Structure(player, id, pos, rot, bldSt, Unit::State::STAND_GROUND){}

	void IceSheet::update(){
		Structure::update();

		if(buildStatus < 100)
			model->getChild(0)->setScale(.01 * buildStatus * Vector3::VEC_IJK);

		for(Player *pl : Game::getSingleton()->getPlayers(true)){
			vector<Unit*> icebreakers = pl->getUnitsByClass(UnitClass::ICEBREAKER);

			for(Unit *icebreaker : icebreakers)
				if(icebreaker->pointWithinObj(pos)){
					player->removeUnit(this);
					return;
				}
		}
	}
}
