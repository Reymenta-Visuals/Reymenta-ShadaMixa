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

	typedef std::shared_ptr<class ChannelsPanel> ChannelsPanelRef;

	class ChannelsPanel
	{

	public:
		ChannelsPanel(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTexturesRef);
		static ChannelsPanelRef create(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTexturesRef);

		void draw();
		void update();
		void resize();

		void toggleVisibility() { mVisible ? hide() : show(); }
		void setChannel1(const int &aChannel, const bool &pressed);
		void setChannel2(const int &aChannel, const bool &pressed);
		void lockEyePointZ(const bool &pressed);
		void tempoEyePointZ(const bool &pressed);
		void resetEyePointZ(const bool &pressed);

		MinimalUI::UIElementRef				buttonChannel1[8], buttonChannel2[8];// , buttonChannel3[8], buttonChannel4[8], buttonChannel5[8], buttonChannel6[8], buttonChannel7[8];
		MinimalUI::UIElementRef				labelChannel1, labelChannel2;// , labelChannel3, labelChannel4, labelChannel5, labelChannel6, labelChannel7;
		MinimalUI::UIElementRef				sliderCamPosXY, sliderCamEyePointXY, sliderCamEyePointZ;
		MinimalUI::UIElementRef				buttonMesh[10];
		MinimalUI::UIElementRef				currentMeshLabel;// , currentRenderShaderLabel, currentPreviewShaderLabel;
		//void setCurrentFbo(const int &aFbo, const bool &pressed);
		void setCurrentMesh(const int &aMesh, const bool &pressed);
		void show();
		void hide();
		//void addFboButtons();
	private:
		void setupParams();
		/*void setCurrentRenderShader(const int &aShaderIndex, const bool &pressed);
		void setCurrentPreviewShader(const int &aShaderIndex, const bool &pressed);*/

		MinimalUI::UIControllerRef mParams;

		ci::app::WindowRef mWindow;
		ci::signals::scoped_connection mCbMouseDown, mCbKeyDown;
		int mPrevState;
		bool mVisible;

		ParameterBagRef				mParameterBag;
		ShadersRef					mShaders;
		TexturesRef					mTextures;

	};
}