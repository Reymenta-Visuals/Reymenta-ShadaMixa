#pragma once

#include "cinder/app/AppBasic.h"
#include "cinder/Timeline.h"
#include "cinder/gl/gl.h"

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

	typedef std::shared_ptr<class SlidersPanel> SlidersPanelRef;

	class SlidersPanel
	{

	public:
		SlidersPanel(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTexturesRef, WindowRef aWindow);
		static SlidersPanelRef create(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTexturesRef, WindowRef aWindow);

		//void setup();
		void draw();
		void update();
		void resize();

		void toggleVisibility() { mVisible ? hide() : show(); }

		void show();
		void hide();
		void resetExposure(const bool &pressed);
		void tempoExposure(const bool &pressed);
		void resetZoom(const bool &pressed);
		void tempoZoom(const bool &pressed);
		void resetRatio(const bool &pressed);
		void tempoRatio(const bool &pressed);
		void resetZPos(const bool &pressed);
		void tempoZPos(const bool &pressed);
		void resetRotationSpeed(const bool &pressed);
		void tempoRotationSpeed(const bool &pressed);
		void lockRatio(const bool &pressed) { autoRatio = pressed; };
		void lockExposure(const bool &pressed) { autoExposure = pressed; };
		void lockZoom(const bool &pressed) { autoZoom = pressed; };
		void lockZPos(const bool &pressed) { autoZPos = pressed; };
		void lockRotationSpeed(const bool &pressed) { autoRotationSpeed = pressed; };

		MinimalUI::UIElementRef	sliderExposure, sliderRatio, sliderZoom, sliderSpeed;//sliderBlendmode, 
		MinimalUI::UIElementRef	sliderZPos, sliderRotationSpeed, sliderSteps, sliderCrossfade, sliderPreviewCrossfade, sliderPixelate;
		// exposure
		float						defaultExposure;
		float						minExposure;
		float						maxExposure;
		bool						tExposure;
		bool						autoExposure;
		//zoom
		float						defaultZoom;
		float						minZoom;
		float						maxZoom;
		bool						tZoom;
		bool						autoZoom;
		//ratio
		float						defaultRatio;
		float						minRatio;
		float						maxRatio;
		bool						tRatio;
		bool						autoRatio;
		// z position
		float						defaultZPos;
		float						minZPos;
		float						maxZPos;
		bool						tZPos;
		bool						autoZPos;
		// Rotation Speed
		float						defaultRotationSpeed;
		float						minRotationSpeed;
		float						maxRotationSpeed;
		bool						tRotationSpeed;
		bool						autoRotationSpeed;

	private:
		void setupParams();

		MinimalUI::UIControllerRef mParams;
		ci::app::WindowRef mWindow;
		ci::signals::scoped_connection mCbMouseDown, mCbKeyDown, mCbLeapConnect, mCbLeapDisconnect;
		int mPrevState;
		bool mVisible;
		string formatNumber(float f);

		ParameterBagRef				mParameterBag;
		ShadersRef					mShaders;
		TexturesRef					mTextures;
	};
}