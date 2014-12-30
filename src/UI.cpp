#include "UI.h"

using namespace Reymenta;
/*
	4:3 w h
	btn: 48 36
	tex: 76 57
	pvw: 156 88
	*/
UI::UI(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTexturesRef, WindowRef aWindow)
{
	mParameterBag = aParameterBag;
	mShaders = aShadersRef;
	mTextures = aTexturesRef;
	mWindow = aWindow;

	mCbMouseDown = mWindow->getSignalMouseDown().connect(0, std::bind(&UI::mouseDown, this, std::placeholders::_1));
	mCbKeyDown = mWindow->getSignalKeyDown().connect(0, std::bind(&UI::keyDown, this, std::placeholders::_1));

	mVisible = true;
	mSetupComplete = false;
	mTimer = 0.0f;
	// tempo
	counter = 0;
	//startTime = timer.getSeconds();
	//currentTime = timer.getSeconds();
	mParameterBag->iDeltaTime = 60 / mParameterBag->mTempo;
	previousTime = 0.0f;
	beatIndex = 0;
	//timer.start();
}

UIRef UI::create(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTexturesRef, app::WindowRef aWindow)
{
	return shared_ptr<UI>(new UI(aParameterBag, aShadersRef, aTexturesRef, aWindow));
}

void UI::setup()
{
	// load custom fonts (I do this once, in my UserInterface class)
	// UI fonts
	mParameterBag->mLabelFont = Font(loadResource(RES_HELVETICA_NEUE_REGULAR), 14 * 2);
	mParameterBag->mSmallLabelFont = Font(loadResource(RES_HELVETICA_NEUE_REGULAR), 12 * 2);
	//mParameterBag->mIconFont = Font(loadResource(RES_GLYPHICONS_REGULAR), 18 * 2);
	mParameterBag->mHeaderFont = Font(loadResource(RES_HELVETICA), 24 * 2);
	mParameterBag->mBodyFont = Font(loadResource(RES_GARAMOND), 19 * 2);
	mParameterBag->mFooterFont = Font(loadResource(RES_GARAMOND_ITALIC), 14 * 2);

	setupGlobal();
	setupMiniControl();
	setupSliders();

	//mVideoPanel = VideoPanel::create(mParameterBag, mTextures, mShaders, "{ \"x\":1060, \"y\":470, \"width\":180, \"height\":190, \"depth\":118, \"fboNumSamples\":0, \"panelColor\":\"0x44282828\" }");
	//mPanels.push_back(mVideoPanel);
	// output to Spout or Syphon
	mOutputPanel = OutputPanel::create(mParameterBag, mTextures);
	mChannelsPanel = ChannelsPanel::create(mParameterBag, mShaders, mTextures);
	mWarpPanel = WarpPanel::create(mParameterBag, mTextures, mShaders);
	//mPanels.push_back(mWarpPanel);
	//mFboPanel = FboPanel::create(mParameterBag, mTextures, mShaders);
	mLibraryPanel = LibraryPanel::create(mParameterBag, mTextures, mShaders);

	mSetupComplete = true;
}
void UI::setupMiniControl()
{
	//mMiniControl = UIController::create("{ \"depth\":100, \"width\":1052, \"fboNumSamples\":2, \"panelColor\":\"0x44282828\", \"height\":180 }");
	mMiniControl = UIController::create("{ \"depth\":100, \"width\":1052, \"fboNumSamples\":0, \"panelColor\":\"0x44282828\", \"height\":174 }");
	mMiniControl->DEFAULT_UPDATE_FREQUENCY = 12;
	mMiniControl->setFont("label", mParameterBag->mLabelFont);
	mMiniControl->setFont("smallLabel", mParameterBag->mSmallLabelFont);
	mMiniControl->setFont("icon", mParameterBag->mIconFont);
	mMiniControl->setFont("header", mParameterBag->mHeaderFont);
	mMiniControl->setFont("body", mParameterBag->mBodyFont);
	mMiniControl->setFont("footer", mParameterBag->mFooterFont);
	mPanels.push_back(mMiniControl);
	//OK mMiniControl->addButton( "S", std::bind(&UI::saveImage, this, std::placeholders::_1 ), "{ \"clear\":false }" );

	// Modes
	// Button Group
	//mMiniControl->addButton("Normal", std::bind(&UI::setMode, this, 0, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"mode\", \"exclusive\":true, \"pressed\":true }");
	mMiniControl->addButton("Mix", std::bind(&UI::setMode, this, 1, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"mode\", \"exclusive\":true, \"pressed\":true }");
	mMiniControl->addButton("Audio", std::bind(&UI::setMode, this, 2, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"mode\", \"exclusive\":true }");
	mMiniControl->addButton("Warp", std::bind(&UI::setMode, this, 3, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"mode\", \"exclusive\":true }");
	mMiniControl->addButton("Sphere", std::bind(&UI::setMode, this, 4, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"mode\", \"exclusive\":true }");
	mMiniControl->addButton("Mesh", std::bind(&UI::setMode, this, 5, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"mode\", \"exclusive\":true }");
	// tempo
	tempoMvg = mMiniControl->addMovingGraphButton("tempo", &mParameterBag->iTempoTime, std::bind(&UI::tapTempo, this, std::placeholders::_1), "{ \"clear\":false,\"width\":76, \"min\":0.0, \"max\":2.001 }");
	sliderTimeFactor = mMiniControl->addSlider("time", &mParameterBag->iTimeFactor, "{ \"min\":0.01, \"max\":32.0, \"clear\":false }");
	for (int i = 0; i < 8; i++)
	{
		mMiniControl->addButton(toString(i), std::bind(&UI::setTimeFactor, this, i, std::placeholders::_1), "{ \"clear\":false, \"width\":9, \"stateless\":false, \"group\":\"timefactor\", \"exclusive\":true }");
	}
	mMiniControl->addButton("time\ntempo", std::bind(&UI::toggleUseTimeWithTempo, this, std::placeholders::_1), "{ \"clear\":false, \"width\":56, \"stateless\":false}");

	fpsMvg = mMiniControl->addMovingGraphButton("fps", &mParameterBag->iFps, std::bind(&UI::showFps, this, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"width\":76, \"min\":0.0, \"max\":120.0 }");
	// ui refresh
	for (int i = 1; i < 6; i++)
	{
		mMiniControl->addButton(toString(i), std::bind(&UI::setUIRefresh, this, i, std::placeholders::_1), "{ \"clear\":false, \"width\":9, \"stateless\":false, \"group\":\"timefactor\", \"exclusive\":true }");
	}

	//sliderUIRefresh = mMiniControl->addSlider("UI\nframes", &mParameterBag->mUIRefresh, "{ \"clear\":false, \"min\":1.0, \"max\":500.0, \"nameColor\":\"0xFFFFFFFF\", \"width\":76 }");
	mMiniControl->addButton("optim\nUI", std::bind(&UI::toggleOptimizeUI, this, std::placeholders::_1), "{ \"clear\":false, \"width\":56, \"stateless\":false}");
	/*
		Panel selector
		*/
	// code panel
	buttonLiveCode = mMiniControl->addButton("midi\nin", std::bind(&UI::useMidiIn, this, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"width\":48 }");
	// output panel
	buttonOutput = mMiniControl->addButton("render\npanel", std::bind(&UI::useOutput, this, std::placeholders::_1), "{ \"width\":48 }");
	// transition
	//buttonTransition = mMiniControl->addButton("transition", std::bind(&UI::useTransition, this, std::placeholders::_1), "{ \"width\":48 }");
	// Audio
	volMvg = mMiniControl->addMovingGraphButton("audio in", &mParameterBag->maxVolume, std::bind(&UI::useLineIn, this, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"width\":48.0, \"min\":0.0, \"max\":500.0 }");
	sliderAudioMul = mMiniControl->addSlider("mul", &mParameterBag->mAudioMultFactor, "{ \"min\":0.01, \"max\":10.0, \"handleVisible\":false, \"width\":76, \"clear\":false }");//\"vertical\":true,
	// Textures select/layers
	// Button Group
	for (int i = 0; i < mTextures->getTextureCount(); i++)
	{
		buttonLayer[i] = mMiniControl->addButton(toString(i), std::bind(&UI::setLayer, this, i, std::placeholders::_1), "{ \"clear\":false, \"width\":48, \"stateless\":false, \"group\":\"layer\", \"exclusive\":true }");
	}

	//labelLayer = mMiniControl->addLabel("iChan0", "{ \"clear\":false,\"width\":48 }");// TODO height not implemented OK\"clear\":false, \"backgroundImage\":\"0.jpg\"
	//mMiniControl->addButton("Sav", std::bind(&UI::saveSettings, this, std::placeholders::_1), "{ \"clear\":false }");
	//mMiniControl->addButton("Ld", std::bind(&UI::restoreSettings, this, std::placeholders::_1), "{ \"clear\":false }");
	mMiniControl->addButton("Rst", std::bind(&UI::resetSettings, this, std::placeholders::_1), "{ \"clear\":false }");

	// windows
	mMiniControl->addButton("Rdr\nPvw", std::bind(&UI::togglePreviewRender, this, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"pressed\":true }");
	//mMiniControl->addButton("Size", std::bind(&UI::selectPreviewSize, this, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"pressed\":true }");
	mMiniControl->addButton("Debug", std::bind(&UI::debug, this, std::placeholders::_1), "{ \"clear\":false, \"width\":56, \"stateless\":false, \"pressed\":false}");
	mMiniControl->addButton("Run\nShader", std::bind(&UI::renderShader, this, std::placeholders::_1), "{ \"width\":56 }");

	// Color Sliders
	mMiniControl->addLabel("Draw color", "{ \"clear\":false }");

	// fr
	sliderRed = mMiniControl->addToggleSlider("R", &mParameterBag->controlValues[1], "a", std::bind(&UI::lockFR, this, std::placeholders::_1), "{ \"width\":36, \"clear\":false, \"handleVisible\":false, \"vertical\":true, \"nameColor\":\"0xEEFF0000\" }", "{ \"width\":9, \"stateless\":false, \"group\":\"fr\", \"exclusive\":true, \"clear\":false }");
	mMiniControl->addButton("t", std::bind(&UI::tempoFR, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"fr\", \"exclusive\":true, \"clear\":false }");
	mMiniControl->addButton("x", std::bind(&UI::resetFR, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"fr\", \"exclusive\":true, \"pressed\":true, \"clear\":false }");

	// fg
	sliderGreen = mMiniControl->addToggleSlider("G", &mParameterBag->controlValues[2], "a", std::bind(&UI::lockFG, this, std::placeholders::_1), "{ \"width\":36, \"clear\":false, \"handleVisible\":false, \"vertical\":true, \"nameColor\":\"0xEE00FF00\" }", "{ \"width\":9, \"stateless\":false, \"group\":\"fg\", \"exclusive\":true, \"clear\":false }");
	mMiniControl->addButton("t", std::bind(&UI::tempoFG, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"fg\", \"exclusive\":true, \"clear\":false }");
	mMiniControl->addButton("x", std::bind(&UI::resetFG, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"fg\", \"exclusive\":true, \"pressed\":true, \"clear\":false }");
	// fb
	sliderBlue = mMiniControl->addToggleSlider("B", &mParameterBag->controlValues[3], "a", std::bind(&UI::lockFB, this, std::placeholders::_1), "{ \"width\":36, \"clear\":false, \"handleVisible\":false, \"vertical\":true, \"nameColor\":\"0xEE0000FF\" }", "{ \"width\":9, \"stateless\":false, \"group\":\"fb\", \"exclusive\":true, \"clear\":false }");
	mMiniControl->addButton("t", std::bind(&UI::tempoFB, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"fb\", \"exclusive\":true, \"clear\":false }");
	mMiniControl->addButton("x", std::bind(&UI::resetFB, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"fb\", \"exclusive\":true, \"pressed\":true, \"clear\":false }");
	// fa
	sliderAlpha = mMiniControl->addToggleSlider("A", &mParameterBag->controlValues[4], "a", std::bind(&UI::lockFA, this, std::placeholders::_1), "{ \"width\":36, \"clear\":false, \"handleVisible\":false, \"vertical\":true, \"nameColor\":\"0xFFFFFFFF\" }", "{ \"width\":9, \"stateless\":false, \"group\":\"fa\", \"exclusive\":true, \"clear\":false }");
	mMiniControl->addButton("t", std::bind(&UI::tempoFA, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"fa\", \"exclusive\":true, \"clear\":false }");
	mMiniControl->addButton("x", std::bind(&UI::resetFA, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"fa\", \"exclusive\":true, \"pressed\":true, \"clear\":false }");

	mMiniControl->addLabel("Back color", "{ \"clear\":false }");
	//br
	sliderBackgroundRed = mMiniControl->addToggleSlider("R", &mParameterBag->controlValues[5], "a", std::bind(&UI::lockBR, this, std::placeholders::_1), "{ \"width\":36, \"clear\":false, \"handleVisible\":false, \"vertical\":true, \"nameColor\":\"0xEEFF0000\" }", "{ \"width\":9, \"stateless\":false, \"group\":\"br\", \"exclusive\":true, \"clear\":false }");
	mMiniControl->addButton("t", std::bind(&UI::tempoBR, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"br\", \"exclusive\":true, \"clear\":false }");
	mMiniControl->addButton("x", std::bind(&UI::resetBR, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"br\", \"exclusive\":true, \"pressed\":true, \"clear\":false }");
	//bg
	sliderBackgroundGreen = mMiniControl->addToggleSlider("G", &mParameterBag->controlValues[6], "a", std::bind(&UI::lockBG, this, std::placeholders::_1), "{ \"width\":36, \"clear\":false, \"handleVisible\":false, \"vertical\":true, \"nameColor\":\"0xEE00FF00\" }", "{ \"width\":9, \"stateless\":false, \"group\":\"bg\", \"exclusive\":true, \"clear\":false }");
	mMiniControl->addButton("t", std::bind(&UI::tempoBG, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"bg\", \"exclusive\":true, \"clear\":false }");
	mMiniControl->addButton("x", std::bind(&UI::resetBG, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"bg\", \"exclusive\":true, \"pressed\":true, \"clear\":false }");
	//bb
	sliderBackgroundBlue = mMiniControl->addToggleSlider("B", &mParameterBag->controlValues[7], "a", std::bind(&UI::lockBB, this, std::placeholders::_1), "{ \"width\":36, \"clear\":false, \"handleVisible\":false, \"vertical\":true, \"nameColor\":\"0xEE0000FF\" }", "{ \"width\":9, \"stateless\":false, \"group\":\"bb\", \"exclusive\":true, \"clear\":false }");
	mMiniControl->addButton("t", std::bind(&UI::tempoBB, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"bb\", \"exclusive\":true, \"clear\":false }");
	mMiniControl->addButton("x", std::bind(&UI::resetBB, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"bb\", \"exclusive\":true, \"pressed\":true, \"clear\":false }");
	//ba
	sliderBackgroundAlpha = mMiniControl->addToggleSlider("A", &mParameterBag->controlValues[8], "a", std::bind(&UI::lockBA, this, std::placeholders::_1), "{ \"width\":36, \"clear\":false, \"handleVisible\":false, \"vertical\":true, \"nameColor\":\"0xFFFFFFFF\" }", "{ \"width\":9, \"stateless\":false, \"group\":\"ba\", \"exclusive\":true, \"clear\":false }");
	mMiniControl->addButton("t", std::bind(&UI::tempoBA, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"ba\", \"exclusive\":true, \"clear\":false }");
	mMiniControl->addButton("x", std::bind(&UI::resetBA, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"ba\", \"exclusive\":true, \"pressed\":true, \"clear\":false }");

	mMiniControl->addButton("grey scale", std::bind(&UI::togglePhong, this, std::placeholders::_1), "{ \"clear\":false, \"width\":72, \"stateless\":false }");
	mMiniControl->addButton("Black", std::bind(&UI::InstantBlack, this, std::placeholders::_1), "{ \"width\":72 }");
	//mMiniControl->addButton("Strobe", std::bind(&UI::Strobe, this, std::placeholders::_1), "{  }");
	// Simple Buttons
	mMiniControl->addButton("chn", std::bind(&UI::toggleChannelPanel, this, std::placeholders::_1), "{ \"clear\":false, \"width\":48 }");
	//mMiniControl->addButton("fbo", std::bind(&UI::toggleFboPanel, this, std::placeholders::_1), "{ \"clear\":false, \"width\":48 }");
	mMiniControl->addButton("lib", std::bind(&UI::toggleLibPanel, this, std::placeholders::_1), "{ \"clear\":false, \"width\":48 }");
	mMiniControl->addButton("wrp", std::bind(&UI::toggleWarpPanel, this, std::placeholders::_1), "{ \"clear\":false, \"width\":48 }");

	mMiniControl->addButton("fade\ncamera", std::bind(&UI::toggleFade, this, std::placeholders::_1), "{ \"clear\":false, \"width\":72, \"stateless\":false }");
	mMiniControl->addButton("light", std::bind(&UI::toggleLight, this, std::placeholders::_1), "{ \"clear\":false, \"width\":72, \"stateless\":false }");
	mMiniControl->addButton("manual light", std::bind(&UI::toggleLightAuto, this, std::placeholders::_1), "{ \"clear\":false, \"width\":72, \"stateless\":false }");
	mMiniControl->addButton("chromatic", std::bind(&UI::chromatic, this, std::placeholders::_1), "{ \"clear\":false,\"width\":72, \"stateless\":false }");

	mMiniControl->addButton("origin\nupper left", std::bind(&UI::toggleOriginUpperLeft, this, std::placeholders::_1), "{ \"clear\":false, \"width\":72, \"stateless\":false }");
	mMiniControl->addButton("repeat", std::bind(&UI::toggleRepeat, this, std::placeholders::_1), "{ \"clear\":false, \"width\":72, \"stateless\":false }");
	mMiniControl->addButton("45 glitch", std::bind(&UI::toggleGlitch, this, std::placeholders::_1), "{ \"clear\":false, \"width\":72, \"stateless\":false }");
	mMiniControl->addButton("46 toggle", std::bind(&UI::toggleAudioReactive, this, std::placeholders::_1), "{ \"clear\":false, \"width\":72, \"stateless\":false }");
	mMiniControl->addButton("47 vignette", std::bind(&UI::toggleVignette, this, std::placeholders::_1), "{ \"clear\":false, \"width\":72, \"stateless\":false }");
	mMiniControl->addButton("48 invert", std::bind(&UI::toggleInvert, this, std::placeholders::_1), "{ \"clear\":false, \"width\":72, \"stateless\":false }");
	mMiniControl->addButton("a", std::bind(&UI::autoInvert, this, std::placeholders::_1), "{ \"clear\":false, \"width\":9, \"stateless\":false }");

	labelOSC = mMiniControl->addLabel("OSC", "{ \"clear\":false, \"width\":100 }");
	labelInfo = mMiniControl->addLabel("Info", "{ \"width\":100 }");
}
void UI::setupGlobal()
{
	gParams = UIController::create("{ \"x\":874, \"y\":456, \"depth\":300, \"width\":400, \"height\":300, \"marginLarge\":2, \"fboNumSamples\":0, \"panelColor\":\"0x44282828\", \"defaultBackgroundColor\":\"0xFF0d0d0d\", \"defaultNameColor\":\"0xFF90a5b6\", \"defaultStrokeColor\":\"0xFF282828\", \"activeStrokeColor\":\"0xFF919ea7\" }", mWindow);
	gParams->DEFAULT_UPDATE_FREQUENCY = 12;
	gParams->setFont("label", mParameterBag->mLabelFont);
	gParams->setFont("smallLabel", mParameterBag->mSmallLabelFont);
	gParams->setFont("icon", mParameterBag->mIconFont);
	gParams->setFont("header", mParameterBag->mHeaderFont);
	gParams->setFont("body", mParameterBag->mBodyFont);
	gParams->setFont("footer", mParameterBag->mFooterFont);
	mPanels.push_back(gParams);

	// 2D Sliders
	//gParams->addSlider2D( "leftRenderXY", &mParameterBag->mLeftRenderXY, "{ \"clear\":false, \"minX\":-2.0, \"maxX\":2.0, \"minY\":-2.0, \"maxY\":2.0, \"width\":" + toString( mParameterBag->mPreviewWidth ) +" }" );
	labelXY = gParams->addLabel("MiddleXY", "{ \"clear\":false, \"width\":" + toString(mParameterBag->mPreviewWidth) + " }");
	labelPosXY = gParams->addLabel("MiddlePosXY", "{ \"width\":" + toString(mParameterBag->mPreviewWidth) + " }");

	sliderRenderXY = gParams->addSlider2D("renderXY", &mParameterBag->mRenderXY, "{ \"clear\":false, \"minX\":-2.0, \"maxX\":2.0, \"minY\":-2.0, \"maxY\":2.0, \"width\":" + toString(mParameterBag->mPreviewWidth) + " }");

	string posXY = toString(mParameterBag->mRenderWidth) + ", \"minY\":" + toString(mParameterBag->mRenderHeight) + ", \"maxY\":0.0, \"width\":" + toString(mParameterBag->mPreviewWidth) + " }";
	sliderRenderPosXY = gParams->addSlider2D("renderPosXY", &mParameterBag->mRenderPosXY, "{ \"minX\":0.0, \"maxX\":" + posXY);

	labelError = gParams->addLabel("no error", "{ \"clear\":false, \"width\":370, \"nameColor\":\"0xFFAA0000\" }");
}
void UI::setupSliders()
{
	mSlidersPanel = SlidersPanel::create(mParameterBag, mShaders, mTextures, mWindow);
}

void UI::setUIRefresh(const int &aFrames, const bool &pressed)
{
	mParameterBag->mUIRefresh = aFrames;
}

void UI::setTimeFactor(const int &aTimeFactor, const bool &pressed)
{
	if (pressed)
	{
		switch (aTimeFactor)
		{
		case 0:
			mParameterBag->iTimeFactor = 0.0001;
			break;
		case 1:
			mParameterBag->iTimeFactor = 0.125;
			break;
		case 2:
			mParameterBag->iTimeFactor = 0.25;
			break;
		case 3:
			mParameterBag->iTimeFactor = 0.5;
			break;
		case 4:
			mParameterBag->iTimeFactor = 0.75;
			break;
		case 5:
			mParameterBag->iTimeFactor = 1.0;
			break;
		case 6:
			mParameterBag->iTimeFactor = 2.0;
			break;
		case 7:
			mParameterBag->iTimeFactor = 4.0;
			break;
		case 8:
			mParameterBag->iTimeFactor = 16.0;
			break;
		default:
			mParameterBag->iTimeFactor = 1.0;
			break;

		}
	}
}

void UI::setLayer(const int &aLayer, const bool &pressed)
{
	if (pressed)
	{
		mParameterBag->currentSelectedIndex = aLayer;
		mParameterBag->iChannels[0] = aLayer;
		mParameterBag->isUIDirty = true;
	}
}

void UI::draw()
{
	// normal alpha blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (mVisible)
	{
		for (auto & panel : mPanels) panel->draw();
		mChannelsPanel->draw();
		mWarpPanel->draw();
		//if ((int)getElapsedSeconds() % 10 == 0) // mParameterBag->mUIRefresh == 0)
		//mFboPanel->draw();
		if (mSlidersPanel) mSlidersPanel->draw();
		if (mTransitionPanel) mTransitionPanel->draw();
		mLibraryPanel->draw();
	}
	mOutputPanel->draw();
	// needed because of what the ping pong fbo is doing, at least
	gl::disableAlphaBlending();

}
string UI::formatNumber(float f)
{
	f *= 100;
	f = (float)((int)f);
	return toString(f);
}
void UI::update()
{
	// check this line position: can't remember
	currentTime = timer.getSeconds();

	int time = (currentTime - startTime)*1000000.0;
	int elapsed = mParameterBag->iDeltaTime*1000000.0;
	if (elapsed > 0)
	{
		double modulo = (time % elapsed) / 1000000.0;
		mParameterBag->iTempoTime = (float)modulo;
		if (mParameterBag->iTempoTime < previousTime)
		{
			beatIndex++;
			if (beatIndex > 3) beatIndex = 0;
		}
		previousTime = mParameterBag->iTempoTime;

		(modulo < 0.1) ? tempoMvg->setNameColor(ColorA::white()) : tempoMvg->setNameColor(UIController::DEFAULT_NAME_COLOR);
		// exposure
		if (mSlidersPanel->tExposure)
		{
			mParameterBag->controlValues[14] = (modulo < 0.1) ? mSlidersPanel->maxExposure : mSlidersPanel->minExposure;
		}
		else
		{
			mParameterBag->controlValues[14] = mSlidersPanel->autoExposure ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, mSlidersPanel->minExposure, mSlidersPanel->maxExposure) : mParameterBag->controlValues[14];
			//mParameterBag->controlValues[14] = mSlidersPanel->autoExposure ? (sin(getElapsedFrames() / (mParameterBag->controlValues[12] + 1.0))) : mParameterBag->controlValues[14];
		}
		// zoom
		if (mSlidersPanel->tZoom)
		{
			mParameterBag->controlValues[13] = (modulo < 0.1) ? mSlidersPanel->maxZoom : mSlidersPanel->minZoom;
		}
		else
		{
			mParameterBag->controlValues[13] = mSlidersPanel->autoZoom ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, mSlidersPanel->minZoom, mSlidersPanel->maxZoom) : mParameterBag->controlValues[13];
		}
		// ratio
		if (mSlidersPanel->tRatio)
		{
			mParameterBag->controlValues[11] = (modulo < 0.1) ? mSlidersPanel->maxRatio : mSlidersPanel->minRatio;
		}
		else
		{
			mParameterBag->controlValues[11] = mSlidersPanel->autoRatio ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, mSlidersPanel->minRatio, mSlidersPanel->maxRatio) : mParameterBag->controlValues[11];
		}
		// RotationSpeed
		if (mSlidersPanel->tRotationSpeed)
		{
			mParameterBag->controlValues[19] = (modulo < 0.1) ? mSlidersPanel->maxRotationSpeed : mSlidersPanel->minRotationSpeed;
		}
		else
		{
			mParameterBag->controlValues[19] = mSlidersPanel->autoRotationSpeed ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, mSlidersPanel->minRotationSpeed, mSlidersPanel->maxRotationSpeed) : mParameterBag->controlValues[19];
		}
		// ZPos
		if (mSlidersPanel->tZPos)
		{
			mParameterBag->mZPosition = (modulo < 0.1) ? mSlidersPanel->maxZPos : mSlidersPanel->minZPos;
		}
		else
		{
			mParameterBag->mZPosition = mSlidersPanel->autoZPos ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, mSlidersPanel->minZPos, mSlidersPanel->maxZPos) : mParameterBag->mZPosition;
		}

		// Front Red
		if (mParameterBag->tFR)
		{
			mParameterBag->controlValues[1] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mParameterBag->controlValues[1] = mParameterBag->mLockFR ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, 0.0, 1.0) : mParameterBag->controlValues[1];
		}
		// Front Green
		if (mParameterBag->tFG)
		{
			mParameterBag->controlValues[2] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mParameterBag->controlValues[2] = mParameterBag->mLockFG ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, 0.0, 1.0) : mParameterBag->controlValues[2];
		}
		// front blue
		if (mParameterBag->tFB)
		{
			mParameterBag->controlValues[3] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mParameterBag->controlValues[3] = mParameterBag->mLockFB ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, 0.0, 1.0) : mParameterBag->controlValues[3];
		}
		// front alpha
		if (mParameterBag->tFA)
		{
			mParameterBag->controlValues[4] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mParameterBag->controlValues[4] = mParameterBag->mLockFA ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, 0.0, 1.0) : mParameterBag->controlValues[4];
		}
		// 
		if (mParameterBag->tBR)
		{
			mParameterBag->controlValues[5] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mParameterBag->controlValues[5] = mParameterBag->mLockBR ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, 0.0, 1.0) : mParameterBag->controlValues[5];
		}
		// 
		if (mParameterBag->tBG)
		{
			mParameterBag->controlValues[6] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mParameterBag->controlValues[6] = mParameterBag->mLockBG ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, 0.0, 1.0) : mParameterBag->controlValues[6];
		}
		// 
		if (mParameterBag->tBB)
		{
			mParameterBag->controlValues[7] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mParameterBag->controlValues[7] = mParameterBag->mLockBB ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, 0.0, 1.0) : mParameterBag->controlValues[7];
		}
		// 
		if (mParameterBag->tBA)
		{
			mParameterBag->controlValues[8] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mParameterBag->controlValues[8] = mParameterBag->mLockBA ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, 0.0, 1.0) : mParameterBag->controlValues[8];
		}
		if (mParameterBag->autoInvert)
		{
			mParameterBag->controlValues[48] = (modulo < 0.1) ? 1.0 : 0.0;
		}

		if (mParameterBag->tEyePointZ)
		{
			mParameterBag->mCamEyePointZ = (modulo < 0.1) ? mParameterBag->minEyePointZ : mParameterBag->maxEyePointZ;
		}
		else
		{
			mParameterBag->mCamEyePointZ = mParameterBag->autoEyePointZ ? lmap<float>(mParameterBag->iTempoTime, 0.00001, mParameterBag->iDeltaTime, mParameterBag->minEyePointZ, mParameterBag->maxEyePointZ) : mParameterBag->mCamEyePointZ;
		}

	}

	for (auto & panel : mPanels) panel->update();
	if (mChannelsPanel) mChannelsPanel->update();
	if (mWarpPanel) mWarpPanel->update();
	//if (mFboPanel) mFboPanel->update();
	if (mSlidersPanel) mSlidersPanel->update();
	if (mTransitionPanel) mTransitionPanel->update();
	if (mOutputPanel->isSetup()) mOutputPanel->update();
	if (mLibraryPanel) mLibraryPanel->update();

	if (getElapsedFrames() % mParameterBag->mUIRefresh * mParameterBag->mUIRefresh * mParameterBag->mUIRefresh == 0)
	{
		if (mVisible)
		{
			if (!mParameterBag->mOptimizeUI)
			{
				tempoMvg->setName(toString(floor(mParameterBag->mTempo)) + "bpm\n" + toString(floor(mParameterBag->iDeltaTime * 1000)) + "ms " + formatNumber(mParameterBag->iTempoTime));
				//audio
				if (mParameterBag->maxVolume < 0.1)
				{
					volMvg->setName("audio in");
				}
				else
				{
					if (mParameterBag->maxVolume < 200.0)
					{
						volMvg->setNameColor(UIController::DEFAULT_NAME_COLOR);
					}
					else
					{
						volMvg->setNameColor(mParameterBag->ColorRed);
					}
					if (mParameterBag->mUseLineIn)
					{
						volMvg->setName("LineIn\n" + toString(floor(mParameterBag->maxVolume)));
					}
					else
					{
						volMvg->setName("Wave\n" + toString(floor(mParameterBag->maxVolume)));
					}
					volMvg->setName(toString(floor(mParameterBag->maxVolume)));
				}
				sliderAudioMul->setName(formatNumber(mParameterBag->mAudioMultFactor));
				sliderTimeFactor->setName(formatNumber(mParameterBag->iTimeFactor));
				if (mParameterBag->iFps > 12.0)
				{
					mParameterBag->FPSColor = UIController::DEFAULT_NAME_COLOR;
				}
				else
				{
					mParameterBag->FPSColor = mParameterBag->ColorRed;
				}
				fpsMvg->setNameColor(mParameterBag->FPSColor);

			}
			if (mParameterBag->controlValues[12] == 0.0) mParameterBag->controlValues[12] = 0.01;

			// iColor slider
			sliderRed->setBackgroundColor(ColorA(mParameterBag->controlValues[1], 0, 0));
			sliderGreen->setBackgroundColor(ColorA(0, mParameterBag->controlValues[2], 0));
			sliderBlue->setBackgroundColor(ColorA(0, 0, mParameterBag->controlValues[3]));
			sliderAlpha->setBackgroundColor(ColorA(mParameterBag->controlValues[1], mParameterBag->controlValues[2], mParameterBag->controlValues[3], mParameterBag->controlValues[4]));
			// iBackColor sliders
			sliderBackgroundRed->setBackgroundColor(ColorA(mParameterBag->controlValues[5], 0, 0));
			sliderBackgroundGreen->setBackgroundColor(ColorA(0, mParameterBag->controlValues[6], 0));
			sliderBackgroundBlue->setBackgroundColor(ColorA(0, 0, mParameterBag->controlValues[7]));
			sliderBackgroundAlpha->setBackgroundColor(ColorA(mParameterBag->controlValues[5], mParameterBag->controlValues[6], mParameterBag->controlValues[7], mParameterBag->controlValues[8]));

			// other sliders
			labelXY->setName(toString(int(mParameterBag->mRenderXY.x * 100) / 100) + "x" + toString(int(mParameterBag->mRenderXY.y * 100) / 100));
			labelPosXY->setName("mouse " + toString(floor(mParameterBag->mRenderPosXY.x)) + "x" + toString(floor(mParameterBag->mRenderPosXY.y)));

			// fps
			fpsMvg->setName(toString(floor(mParameterBag->iFps)) + " fps");

			labelOSC->setName(mParameterBag->OSCMsg);
			labelInfo->setName(mParameterBag->InfoMsg);
			labelError->setName(mShaders->getFragError());
			if (mParameterBag->isUIDirty)
			{
				// do this once
				mParameterBag->isUIDirty = false;
			}
			for (int i = 0; i < mTextures->getTextureCount(); i++)
			{
				buttonLayer[i]->setBackgroundTexture(mTextures->getTexture(i));
			}
		}
	}
}
void UI::tempoFR(const bool &pressed)
{
	mParameterBag->tFR = pressed;
	if (!pressed) resetFR(pressed);
}
void UI::resetFR(const bool &pressed)
{
	mParameterBag->mLockFR = mParameterBag->tFR = false;
}
void UI::tempoFG(const bool &pressed)
{
	mParameterBag->tFG = pressed;
	if (!pressed) resetFG(pressed);
}
void UI::resetFG(const bool &pressed)
{
	mParameterBag->mLockFG = mParameterBag->tFG = false;
}
void UI::tempoFB(const bool &pressed)
{
	mParameterBag->tFB = pressed;
	if (!pressed) resetFB(pressed);
}
void UI::resetFB(const bool &pressed)
{
	mParameterBag->mLockFB = mParameterBag->tFB = false;
}
void UI::tempoFA(const bool &pressed)
{
	mParameterBag->tFA = pressed;
	if (!pressed) resetFA(pressed);
}
void UI::resetFA(const bool &pressed)
{
	mParameterBag->mLockFA = mParameterBag->tFA = false;
}
void UI::tempoBR(const bool &pressed)
{
	mParameterBag->tBR = pressed;
	if (!pressed) resetBR(pressed);
}
void UI::resetBR(const bool &pressed)
{
	mParameterBag->mLockBR = mParameterBag->tBR = false;
}
void UI::tempoBG(const bool &pressed)
{
	mParameterBag->tBG = pressed;
	if (!pressed) resetBG(pressed);
}
void UI::resetBG(const bool &pressed)
{
	mParameterBag->mLockBG = mParameterBag->tBG = false;
}
void UI::tempoBB(const bool &pressed)
{
	mParameterBag->tBB = pressed;
	if (!pressed) resetBB(pressed);
}
void UI::resetBB(const bool &pressed)
{
	mParameterBag->mLockBB = mParameterBag->tBB = false;
}
void UI::tempoBA(const bool &pressed)
{
	mParameterBag->tBA = pressed;
	if (!pressed) resetBA(pressed);
}
void UI::resetBA(const bool &pressed)
{
	mParameterBag->mLockBA = mParameterBag->tBA = false;
}

void UI::resize()
{
	for (auto & panel : mPanels) panel->resize();
	if (mChannelsPanel) mChannelsPanel->resize();
	if (mWarpPanel) mWarpPanel->resize();
	if (mSlidersPanel) mSlidersPanel->resize();
	if (mTransitionPanel) mTransitionPanel->resize();
	if (mLibraryPanel) mLibraryPanel->resize();
}

void UI::mouseDown(MouseEvent &event)
{
}

void UI::keyDown(KeyEvent &event)
{
	/*switch (event.getChar())
	{
	// toggle params & mouse
	case 'h':
	toggleVisibility();
	break;
	}*/
}

void UI::show()
{
	mVisible = true;
	AppBasic::get()->showCursor();
}

void UI::hide()
{
	mVisible = false;
	//AppBasic::get()->hideCursor();
}
void UI::InstantBlack(const bool &pressed)
{
	mParameterBag->controlValues[1] = mParameterBag->controlValues[2] = mParameterBag->controlValues[3] = mParameterBag->controlValues[4] = 0.0;
	mParameterBag->controlValues[5] = mParameterBag->controlValues[6] = mParameterBag->controlValues[7] = mParameterBag->controlValues[8] = 0.0;

}
void UI::Strobe(const bool &pressed)
{
}
void UI::togglePreviewRender(const bool &pressed)
{
	mParameterBag->mPreviewEnabled = pressed;
}
void UI::renderShader(const bool &pressed)
{
	mShaders->renderPreviewShader();
}
void UI::toggleFade(const bool &pressed)
{
	mParameterBag->iFade = pressed;
}
void UI::toggleGlitch(const bool &pressed)
{
	mParameterBag->controlValues[45] = pressed;
}
void UI::toggleAudioReactive(const bool &pressed)
{
	mParameterBag->controlValues[46] = pressed;
}
void UI::toggleVignette(const bool &pressed)
{
	mParameterBag->controlValues[47] = pressed;
}
void UI::toggleInvert(const bool &pressed)
{
	mParameterBag->controlValues[48] = pressed;
}
void UI::toggleUseTimeWithTempo(const bool &pressed)
{
	mParameterBag->mUseTimeWithTempo = pressed;
}
void UI::toggleOptimizeUI(const bool &pressed)
{
	mParameterBag->mOptimizeUI = pressed;
}
void UI::autoInvert(const bool &pressed)
{
	mParameterBag->autoInvert = pressed;
}
void UI::toggleRepeat(const bool &pressed)
{
	mParameterBag->iRepeat = pressed;
}
void UI::toggleLight(const bool &pressed)
{
	mParameterBag->iLight = pressed;
}
void UI::toggleLightAuto(const bool &pressed)
{
	mParameterBag->iLightAuto = pressed;
}
void UI::togglePhong(const bool &pressed)
{
	mParameterBag->iGreyScale = pressed;
}
void UI::toggleOriginUpperLeft(const bool &pressed)
{
	mParameterBag->mOriginUpperLeft = pressed;
}
void UI::useOutput(const bool &pressed)
{
	mOutputPanel->toggleVisibility();
}
void UI::useMidiIn(const bool &pressed)
{
	if (!pressed)
	{
		show();
	}
	mParameterBag->mWindowToCreate = MIDI_IN;
}
void UI::channelsSelector(const bool &pressed)
{
	mChannelsPanel->toggleVisibility();
}

void UI::useLineIn(const bool &pressed)
{
	mParameterBag->mUseLineIn = pressed;
}
//void UI::useTransition(const bool &pressed)
//{
//	if (mTransitionPanel == NULL)
//	{
//		mTransitionPanel = TransitionPanel::create(mParameterBag, mShaders, mTextures, mWindow);
//	}
//	mTransitionPanel->toggleVisibility();
//}
void UI::chromatic(const bool &pressed)
{
	mParameterBag->controlValues[15] = pressed;
}
void UI::changeMode(int newMode)
{
	if (mParameterBag->mMode != newMode)
	{

		mParameterBag->controlValues[4] = 1.0f;
		mParameterBag->controlValues[8] = 1.0f;

		mParameterBag->mPreviousMode = mParameterBag->mMode;
		switch (mParameterBag->mPreviousMode)
		{
		case 3: //warp
			mWarpPanel->toggleVisibility();
			break;
		case 5: //mesh
			mParameterBag->iLight = false;
			break;
		}
		mParameterBag->mMode = newMode;
		switch (newMode)
		{
		case 3: //warp
			mWarpPanel->toggleVisibility();
			break;
		case 4: //sphere
			mParameterBag->mCamPosXY = Vec2f(-155.6, -87.3);
			mParameterBag->mCamEyePointZ = -436.f;
			mParameterBag->controlValues[5] = mParameterBag->controlValues[6] = mParameterBag->controlValues[7] = 0;
			break;
		case 5: //mesh
			mParameterBag->mRenderPosXY = Vec2f(0.0, 0.0);
			mParameterBag->mCamEyePointZ = -56.f;
			mParameterBag->controlValues[5] = mParameterBag->controlValues[6] = mParameterBag->controlValues[7] = 0;
			mParameterBag->currentSelectedIndex = 5;
			mParameterBag->iLight = true;
			break;
		}
	}
}
void UI::setMode(const int &aMode, const bool &pressed)
{

	if (pressed) mParameterBag->mNewMode = aMode;

}
// show Fps
void UI::showFps(const bool &pressed)
{
	mParameterBag->iShowFps = pressed;
}
// debug
void UI::debug(const bool &pressed)
{
	mParameterBag->iDebug = pressed;
}
// tempo
void UI::tapTempo(const bool &pressed)
{
	startTime = currentTime = timer.getSeconds();

	timer.stop();
	timer.start();

	// check for out of time values - less than 50% or more than 150% of from last "TAP and whole time budder is going to be discarded....
	if (counter > 2 && (buffer.back() * 1.5 < currentTime || buffer.back() * 0.5 > currentTime))
	{
		buffer.clear();
		counter = 0;
		averageTime = 0;
	}
	if (counter >= 1)
	{
		buffer.push_back(currentTime);
		calculateTempo();
	}
	counter++;
}
void UI::calculateTempo()
{
	// NORMAL AVERAGE
	double tAverage = 0;
	for (int i = 0; i < buffer.size(); i++)
	{
		tAverage += buffer[i];
	}
	averageTime = (double)(tAverage / buffer.size());
	mParameterBag->iDeltaTime = averageTime;
	mParameterBag->mTempo = 60 / averageTime;
}
/*void UI::selectPreviewSize(const bool &pressed)
{
mParameterBag->mPreviewLargeSize = pressed;
mTextures->createPreviewFbo();
}
void UI::saveSettings(const bool &pressed)
{
	if (mParameterBag->save()) {
	}
	else
	{
	}
}

void UI::restoreSettings(const bool &pressed)
{
	if (mParameterBag->restore()) {
	}
	else
	{
	}
}*/

void UI::resetSettings(const bool &pressed)
{
	mParameterBag->reset();
}
void UI::shutdown()
{
	if (mOutputPanel->isSetup()) mOutputPanel->shutdown();
}