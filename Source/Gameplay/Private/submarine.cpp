#include "submarine.h"
#include "map.h"

#include <quad.h>

namespace battleship{
	using namespace vb01;
	using namespace std;

	Submarine::Submarine(Player *player, int id, Vector3 pos, Quaternion rot, Unit::State state) : Vehicle(player, id, pos, rot, state){}

	bool Submarine::validateLaunchOrder(){
		if(!Unit::validateLaunchOrder()) return false;

		vector<Node*> terrainNodes = Map::getSingleton()->getNodeParent()->getChildren();

		for(int i = 1; i < terrainNodes.size(); i++){
			Vector3 wbPos = terrainNodes[i]->getPosition();
			Vector3 size = ((Quad*)terrainNodes[i]->getMesh(0))->getSize();

			if(fabs(wbPos.x - pos.x) < .5 * size.x && fabs(wbPos.z - pos.z) < .5 * size.y && pos.y - wbPos.y > -.1)
				return true;
		}

		return false;
	}
}
