#include "LibraryPanel.h"

using namespace Reymenta;
/*
4:3 w h
btn: 48 36
tex: 76 57
pvw: 156 88
*/
LibraryPanel::LibraryPanel(ParameterBagRef aParameterBag, TexturesRef aTexturesRef, ShadersRef aShadersRef)
{
	mParameterBag = aParameterBag;
	mShaders = aShadersRef;
	mTextures = aTexturesRef;

	setupParams();
}

LibraryPanelRef LibraryPanel::create(ParameterBagRef aParameterBag, TexturesRef aTexturesRef, ShadersRef aShadersRef)
{
	return shared_ptr<LibraryPanel>(new LibraryPanel(aParameterBag, aTexturesRef, aShadersRef));
}

void LibraryPanel::setupParams()
{
	mParams = UIController::create("{ \"visible\":true, \"x\":220, \"y\":176, \"width\":650, \"height\":600, \"depth\":203, \"panelColor\":\"0x44282828\" }");
	mParams->DEFAULT_UPDATE_FREQUENCY = 12;
	// set custom fonts for a UIController
	mParams->setFont("label", mParameterBag->mLabelFont);
	mParams->setFont("smallLabel", mParameterBag->mSmallLabelFont);
	mParams->setFont("icon", mParameterBag->mIconFont);
	mParams->setFont("header", mParameterBag->mHeaderFont);
	mParams->setFont("body", mParameterBag->mBodyFont);
	mParams->setFont("footer", mParameterBag->mFooterFont);

	sliderLeftRenderXY = mParams->addSlider2D("LeftXY", &mParameterBag->mLeftRenderXY, "{ \"clear\":false, \"minX\":-0.5, \"maxX\":0.5, \"minY\":-0.5, \"maxY\":0.5, \"width\":" + toString(mParameterBag->mPreviewWidth) + " }");
	sliderRightRenderXY = mParams->addSlider2D("RightXY", &mParameterBag->mRightRenderXY, "{ \"clear\":false, \"minX\":-0.5, \"maxX\":0.5, \"minY\":-0.5, \"maxY\":0.5, \"width\":" + toString(mParameterBag->mPreviewWidth) + " }");
	sliderMixRenderXY = mParams->addSlider2D("MixXY", &mParameterBag->mPreviewRenderXY, "{ \"clear\":false, \"minX\":-0.5, \"maxX\":0.5, \"minY\":-0.5, \"maxY\":0.5, \"width\":" + toString(mParameterBag->mPreviewWidth) + " }");
	sliderPreviewRenderXY = mParams->addSlider2D("PreviewFragXY", &mParameterBag->mPreviewFragXY, "{ \"minX\":-0.5, \"maxX\":0.5, \"minY\":-0.5, \"maxY\":0.5, \"width\":" + toString(mParameterBag->mPreviewWidth) + " }");
	mParams->addSlider("LZoom", &mParameterBag->iZoomLeft, "{ \"clear\":false, \"width\":" + toString(mParameterBag->mPreviewWidth) + ", \"min\":0.1, \"max\":2.0 }");
	mParams->addSlider("RZoom", &mParameterBag->iZoomRight, "{ \"clear\":false, \"width\":" + toString(mParameterBag->mPreviewWidth) + ", \"min\":0.1, \"max\":2.0 }");
	flipButton = mParams->addButton("Flip", std::bind(&LibraryPanel::flipLibraryCurrentFbo, this, std::placeholders::_1), "{ \"width\":48, \"stateless\":false, \"pressed\":true }");
	// create 10 frag btns
	for (int i = 0; i < 10; i++)
	{
		buttonFrag.push_back(mParams->addButton(toString(i), std::bind(&LibraryPanel::setCurrentFbo, this, i + 8, std::placeholders::_1), "{ \"clear\":false, \"width\":48, \"stateless\":false, \"group\":\"fbolib\", \"exclusive\":true }"));
		mParams->addButton("L", std::bind(&LibraryPanel::setLeftFragActive, this, i, std::placeholders::_1), "{ \"clear\":false, \"width\":18, \"stateless\":false, \"group\":\"c0\", \"exclusive\":true }");
		mParams->addButton("R", std::bind(&LibraryPanel::setRightFragActive, this, i, std::placeholders::_1), "{ \"clear\":false, \"width\":18, \"stateless\":false, \"group\":\"c1\", \"exclusive\":true }");
		buttonPreview.push_back(mParams->addButton("P", std::bind(&LibraryPanel::setPreviewActive, this, i, std::placeholders::_1), "{ \"width\":18, \"stateless\":false, \"group\":\"c2\", \"exclusive\":true }"));
	}
	buttonPreview[2]->press();

}
void LibraryPanel::flipLibraryCurrentFbo(const bool &pressed)
{
	mTextures->flipMixFbo(pressed);
}
void LibraryPanel::setCurrentFbo(const int &aIndex, const bool &pressed)
{
	//if (pressed) mParameterBag->mCurrentFboLibraryIndex = aIndex;
}
void LibraryPanel::setLeftFragActive(const int &aIndex, const bool &pressed)
{
	if (pressed) mParameterBag->mLeftFragIndex = aIndex;
}
void LibraryPanel::setRightFragActive(const int &aIndex, const bool &pressed)
{
	if (pressed) mParameterBag->mRightFragIndex = aIndex;
}
void LibraryPanel::setPreviewActive(const int &aIndex, const bool &pressed)
{
	if (pressed) mParameterBag->mPreviewFragIndex = aIndex;
}
void LibraryPanel::update()
{
	// animation	
	if (mVisible)
	{
		mParams->update();

		if (!mParameterBag->mOptimizeUI)
		{
			if (getElapsedFrames() % (mParameterBag->mUIRefresh * mParameterBag->mUIRefresh * mParameterBag->mUIRefresh) == 0)
			{
				sliderLeftRenderXY->setBackgroundTexture(mTextures->getFboTexture(mParameterBag->mLeftFboIndex));
				sliderRightRenderXY->setBackgroundTexture(mTextures->getFboTexture(mParameterBag->mRightFboIndex));
				sliderMixRenderXY->setBackgroundTexture(mTextures->getFboTexture(mParameterBag->mMixFboIndex));
				sliderPreviewRenderXY->setBackgroundTexture(mTextures->getFboTexture(mParameterBag->mCurrentPreviewFboIndex));
				//buttonFrag[mParameterBag->mPreviewFragIndex]->setBackgroundTexture(mTextures->getFboThumb(mParameterBag->mCurrentPreviewFboIndex));
				if (mParameterBag->mPreviewFragIndex != mParameterBag->mPreviousFragIndex)
				{
					buttonPreview[mParameterBag->mPreviousFragIndex]->release();				
					buttonPreview[mParameterBag->mPreviewFragIndex]->press();
					mParameterBag->mPreviousFragIndex = mParameterBag->mPreviewFragIndex;
				}
			}
		}
	}
}
void LibraryPanel::draw()
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

void LibraryPanel::resize()
{
	mParams->resize();
}

void LibraryPanel::show()
{
	mVisible = true;
}

void LibraryPanel::hide()
{
	mVisible = false;
}
