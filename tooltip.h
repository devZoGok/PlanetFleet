#ifndef TOOLTIP_H
#define TOOLTIP_H

#include <vector.h>

#include <vector>
#include <utility>
#include <string>

namespace vb01{
	class Quad;
	class Text;
}

namespace battleship{
    class Tooltip{
    public:
		struct LineData{
			int height;
			float scale;
			vb01::Vector3 textOffset;
			std::wstring entryText;
			vb01::Vector4 textColor;
			std::vector<std::pair<int, std::string>> iconsData;

			inline vb01::Vector3 getLineOffset(int i){return vb01::Vector3(textOffset.x, height * (i + 1) + textOffset.y, textOffset.z);}

			LineData(std::wstring et, vb01::Vector4 tc, std::vector<std::pair<int, std::string>> id, int h, vb01::Vector3 to, float s) : 
				entryText(et), 
				textColor(tc), 
				iconsData(id), 
				height(h), 
				textOffset(to),
				scale(s)
			{}
		};

        Tooltip(vb01::Vector3, vb01::Vector2, std::vector<LineData>, std::string);
        ~Tooltip();
        void update();
		inline vb01::Quad* getBackground(){return background;}
    private:
        vb01::Vector3 pos;
        vb01::Vector2 size;
		vb01::Quad *background = nullptr;
    };
}

#endif
