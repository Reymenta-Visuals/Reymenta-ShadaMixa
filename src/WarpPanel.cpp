#include "WarpPanel.h"

using namespace Reymenta;
/*
4:3 w h
btn: 48 36
tex: 76 57
pvw: 156 88

*/
WarpPanel::WarpPanel(ParameterBagRef aParameterBag, TexturesRef aTexturesRef, ShadersRef aShadersRef)
{
	mParameterBag = aParameterBag;
	mTextures = aTexturesRef;
	mShaders = aShadersRef;
	// Textures index channel 1
	warpIndex = 0;
	mVisible = false;
	setupParams();
}

WarpPanelRef WarpPanel::create(ParameterBagRef aParameterBag, TexturesRef aTexturesRef, ShadersRef aShadersRef)
{
	return shared_ptr<WarpPanel>(new WarpPanel(aParameterBag, aTexturesRef, aShadersRef));
}
void WarpPanel::setupParams()
{
	mParams = UIController::create("{ \"visible\":true, \"x\":730, \"y\":176, \"width\":140, \"height\":530, \"depth\":203, \"panelColor\":\"0x44282828\" }");
	mParams->DEFAULT_UPDATE_FREQUENCY = 12;
	// set custom fonts for a UIController
	mParams->setFont("label", mParameterBag->mLabelFont);
	mParams->setFont("smallLabel", mParameterBag->mSmallLabelFont);
	mParams->setFont("icon", mParameterBag->mIconFont);
	mParams->setFont("header", mParameterBag->mHeaderFont);
	mParams->setFont("body", mParameterBag->mBodyFont);
	mParams->setFont("footer", mParameterBag->mFooterFont);
	// warp index
	currentIndexLabel = mParams->addLabel("Select\nWarp", "{ \"clear\":false, \"width\":48 }");
	mParams->addButton("+", std::bind(&WarpPanel::setCurrentFboIndex, this, 1, std::placeholders::_1), "{ \"clear\":false, \"width\":36 }");
	mParams->addButton("-", std::bind(&WarpPanel::setCurrentFboIndex, this, -1, std::placeholders::_1), "{  \"width\":36 }");
	for (int i = 0; i < 8; i++)
	{
		buttonIndex[i] = mParams->addButton(toString(i), std::bind(&WarpPanel::setCurrentIndex, this, i, std::placeholders::_1), "{ \"clear\":false, \"width\":48, \"stateless\":false, \"group\":\"index\", \"exclusive\":true }");
		labelFboIndex[i] = mParams->addLabel(toString(mParameterBag->mWarpFbos[warpIndex].textureIndex), "{ \"width\":48 }");
	}
}
void WarpPanel::setCurrentIndex(const int &aIndex, const bool &pressed)
{
	if (pressed)
	{
		warpIndex = aIndex;
		currentIndexLabel->setName("Warp\n" + toString(aIndex));
	}
}
void WarpPanel::setCurrentFboIndex(const int &aIndex, const bool &pressed)
{
	mParameterBag->mWarpFbos[warpIndex].textureIndex += aIndex;
	if (mParameterBag->mWarpFbos[warpIndex].textureIndex > 7) mParameterBag->mWarpFbos[warpIndex].textureIndex = 0;
	if (mParameterBag->mWarpFbos[warpIndex].textureIndex < 0) mParameterBag->mWarpFbos[warpIndex].textureIndex = 7;
	labelFboIndex[warpIndex]->setName(toString(mParameterBag->mWarpFbos[warpIndex].textureIndex));
}
void WarpPanel::update()
{
	// animation
	if (mVisible && !mParameterBag->mOptimizeUI)
	{
		mParams->update();
		for (int i = 0; i < 8; i++)
		{
			buttonIndex[i]->setBackgroundTexture(mTextures->getFboTexture(mParameterBag->mWarpFbos[warpIndex].textureIndex));
		}
	}
}
void WarpPanel::draw()
{
	if (mVisible)
	{
		// normal alpha blending
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		mParams->draw();

		// needed because of what the ping pong fbo is doing, at least
		gl::disableAlphaBlending();
	}
}
void WarpPanel::resize()
{
	mParams->resize();
}
void WarpPanel::show()
{
	mVisible = true;
}
void WarpPanel::hide()
{
	mVisible = false;
}