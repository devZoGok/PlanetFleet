#include "pfButtonBase.h"
#include "tooltip.h"

#include <quad.h>
#include <node.h>

namespace battleship{
	using namespace std;
	using namespace vb01;

	PfButtonBase::PfButtonBase(Vector3 pos, Vector2 size, string name, string fontPath, int trigger, bool separate, string imagePath) : Button(pos, size, name, fontPath, trigger, separate, imagePath){}

	PfButtonBase::~PfButtonBase(){
		if(tooltip) delete tooltip;
	}

	void PfButtonBase::update() {
		if(enabledHoveredOn)
			setColor(mouseOver ? hoveredOnColor : baseColor);

		if(tooltip) tooltip->getBackground()->getNode()->setVisible(mouseOver);
	}
}
