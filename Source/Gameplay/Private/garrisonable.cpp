#include "garrisonable.h"
#include "unit.h"

#include <node.h>

namespace battleship{
	using namespace vb01;

	void Garrisonable::update(){
	}

	void Garrisonable::ejectVehicle(int id){
	}

	void Garrisonable::prepareGarrisonSlots(){
		for(int i = 0; i < capacity; i++){
			Vector2 size = 10 * Vector2::VEC_IJ;
			Vector2 pos = Vector2(0, 10 * (i));
			garrisonSlotBackgrounds.push_back(Unit::createBar(pos, size, Vector4(0, 0, 0, 1)));
			garrisonSlotForegrounds.push_back(Unit::createBar(pos, size, Vector4(0, 1, 0, 1)));
		}
	}
}
