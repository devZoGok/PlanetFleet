#ifndef EXPORT_BUTTON_H
#define EXPORT_BUTTON_H

#include <button.h>

namespace battleship{
	class ExportButton : public vb01Gui::Button{
		public:
			ExportButton(vb01::Vector3 pos, vb01::Vector2 size);
			void onClick();
	};
}

#endif

