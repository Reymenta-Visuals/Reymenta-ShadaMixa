#pragma once

#include "cinder/app/AppBasic.h"
#include "cinder/Timeline.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/TextureFont.h"

#include "Resources.h"
#include "ParameterBag.h"
#include "UIController.h"
#include "UIElement.h"
// shaders
#include "Shaders.h"
// textures
#include "Textures.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MinimalUI;

namespace Reymenta
{

	typedef std::shared_ptr<class LibraryPanel> LibraryPanelRef;

	class LibraryPanel
	{

	public:
		LibraryPanel(ParameterBagRef aParameterBag, TexturesRef aTexturesRef, ShadersRef aShadersRef);
		static LibraryPanelRef create(ParameterBagRef aParameterBag, TexturesRef aTexturesRef, ShadersRef aShadersRef);

		void draw();
		void update();
		void resize();

		void toggleVisibility() { mVisible ? hide() : show(); }
		void setCurrentFbo(const int &aIndex, const bool &pressed);

		void show();
		void hide();
		//void addShader(std::string fullPath);
		vector<MinimalUI::UIElementRef>	buttonPreview;
	private:
		void setupParams();
		void flipV(const bool &pressed);
		void flipH(const bool &pressed);
		//void setLeftFboActive(const int &aIndex, const bool &pressed);
		//void setRightFboActive(const int &aIndex, const bool &pressed);
		void setLeftFragActive(const int &aIndex, const bool &pressed);
		void setRightFragActive(const int &aIndex, const bool &pressed);
		void setPreviewActive(const int &aIndex, const bool &pressed);
		//void setFboActive(const int &aIndex, const bool &pressed);
		vector<MinimalUI::UIElementRef>	buttonFrag, buttonActive;
		
		MinimalUI::UIElementRef			flipVButton, flipHButton, sliderLeftRenderXY, sliderRightRenderXY, sliderMixRenderXY, sliderPreviewRenderXY;
		MinimalUI::UIControllerRef		mParams;
		bool							mVisible;

		ParameterBagRef					mParameterBag;
		ShadersRef						mShaders;
		TexturesRef						mTextures;
	};
}