#ifndef PF_BUTTON_BASE_H
#define PF_BUTTON_BASE_H

#include <button.h>

namespace battleship{
	class Tooltip;

	class PfButtonBase : public vb01Gui::Button{
		public:
			PfButtonBase(vb01::Vector3, vb01::Vector2, std::string, std::string, int, bool, std::string = "");
			~PfButtonBase();
			virtual void update();
			inline void setTooltip(Tooltip *t){this->tooltip = t;}
			inline void setEnabledHoverdOn(bool e){this->enabledHoveredOn = e;}
			inline void setHoveredOnColor(vb01::Vector4 hc){this->hoveredOnColor = hc;}
			inline void setBaseColor(vb01::Vector4 bc){this->baseColor = bc;}
		protected:
			bool enabledHoveredOn = false;
			vb01::Vector4 baseColor, hoveredOnColor;
			Tooltip *tooltip = nullptr;
	};
}

#endif
