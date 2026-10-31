#include "tooltip.h"
#include "gameManager.h"

#include <root.h>
#include <text.h>
#include <quad.h>
#include <node.h>
#include <material.h>
#include <texture.h>

using namespace std;
using namespace vb01;

namespace battleship{
    Tooltip::Tooltip(Vector3 p, Vector2 s, vector<Tooltip::LineData> lineData, string fontPath) : pos(p), size(s){
		Root *root = Root::getSingleton();
		Material *bgMat = new Material(root->getLibPath() + "gui");
		bgMat->addBoolUniform("texturingEnabled", false);
		bgMat->addVec4Uniform("diffuseColor", Vector4(.5, .5, .5, 1));

		background = new Quad(Vector3(s.x, s.y, 0), false);
		background->setMaterial(bgMat);

		Node *bgNode = new Node(p);
		bgNode->attachMesh(background);
		root->getGuiNode()->attachChild(bgNode);

		string basePath = GameManager::getSingleton()->getPath();
		string iconBasePath = basePath + "Textures/Icons/";

		for(int i = 0; i < lineData.size(); i++){
			LineData &ld = lineData[i];

			Material *textMat = new Material(root->getLibPath() + "text");
			textMat->addBoolUniform("texturingEnabled", false);
			textMat->addVec4Uniform("diffuseColor", ld.textColor);

			Vector3 offset = ld.getLineOffset(i);
			Text *text = new Text(fontPath, ld.entryText);
			text->setMaterial(textMat);
			Node *textNode = new Node(pos + offset, Quaternion::QUAT_W, ld.scale * Vector3::VEC_IJK);
			textNode->addText(text);
			bgNode->attachChild(textNode);

			for(pair<int, string> &iconData: ld.iconsData){
				Vector3 iconOffset = offset;
				wstring entry = text->getText();

				for(int j = 0; j < iconData.first; j++){
					Text::Glyph *glyph = text->getGlyph(entry[j]);
					iconOffset.x += ld.scale * (glyph->advance >> 6);
				}

				entry.insert(iconData.first, L"  ");
				text->setText(entry);

				Material *iconMat = new Material(root->getLibPath() + "gui");
				iconMat->addBoolUniform("texturingEnabled", true);
				string ip[]{iconBasePath + iconData.second};
				Texture *t = new Texture(ip, 1, false);
				iconMat->addTexUniform("textures[0]", t, false);

				Vector2 iconSize = ld.height * Vector2::VEC_IJ;
				Quad *rect = new Quad(Vector3(iconSize.x, iconSize.y, 0), false);
				rect->setMaterial(iconMat);
				Node *rectNode = new Node(iconOffset - Vector3(0, iconSize.y, 0));
				rectNode->attachMesh(rect);
				bgNode->attachChild(rectNode);
			}
		}
    }
    
    Tooltip::~Tooltip(){
		Node *node = background->getNode();
		Root::getSingleton()->getGuiNode()->dettachChild(node);
		delete node;
    }
    
    void Tooltip::update(){
    }
}
