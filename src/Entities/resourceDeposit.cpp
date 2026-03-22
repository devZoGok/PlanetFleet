#include "resourceDeposit.h"

namespace battleship{
	using namespace vb01;

	ResourceDeposit::ResourceDeposit(Player *player, int id, Vector3 pos, Quaternion rot, int ia) : 
		GameObject(GameObject::Type::RESOURCE_DEPOSIT, id, player, pos, rot),
		initAmmount(ia),
		ammount(ia)
	{
		initProperties();
		initModel();
		initHitbox();
		placeAt(pos);
		orientAt(rot);
	}

	ResourceDeposit::~ResourceDeposit(){
		destroyHitbox();
		destroyModel();
	}

	void ResourceDeposit::reinit(){
		destroyHitbox();
		destroyModel();

		initProperties();

		initModel();
		initHitbox();

		GameObject::reinit();
	}
}
