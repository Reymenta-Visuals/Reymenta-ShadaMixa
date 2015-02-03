#include "ChannelsPanel.h"

using namespace Reymenta;
/*
4:3 w h
btn: 48 36
tex: 76 57
pvw: 156 88
*/
ChannelsPanel::ChannelsPanel(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTexturesRef)
{
	mParameterBag = aParameterBag;
	mShaders = aShadersRef;
	mTextures = aTexturesRef;

	mVisible = false;
	setupParams();
}

ChannelsPanelRef ChannelsPanel::create(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTexturesRef)
{
	return shared_ptr<ChannelsPanel>(new ChannelsPanel(aParameterBag, aShadersRef, aTexturesRef));
}

void ChannelsPanel::setupParams()
{
	mParams = UIController::create("{ \"visible\":true, \"x\":0, \"y\":176, \"width\":208, \"height\":530, \"depth\":203, \"panelColor\":\"0x44282828\" }");
	mParams->DEFAULT_UPDATE_FREQUENCY = 12;
	// set custom fonts for a UIController
	mParams->setFont("label", mParameterBag->mLabelFont);
	mParams->setFont("smallLabel", mParameterBag->mSmallLabelFont);
	mParams->setFont("icon", mParameterBag->mIconFont);
	mParams->setFont("header", mParameterBag->mHeaderFont);
	mParams->setFont("body", mParameterBag->mBodyFont);
	mParams->setFont("footer", mParameterBag->mFooterFont);


	// Textures index channel 1
	mParams->addLabel("iChan1", "{ \"width\":64, \"clear\":false }");
	// Button Group for iChannel1
	for (int i = 1; i < mTextures->getTextureCount(); i++)
	{
		buttonChannel1[i] = mParams->addButton(toString(i), std::bind(&ChannelsPanel::setChannel1, this, i, std::placeholders::_1), "{ \"clear\":false, \"width\":9, \"stateless\":false, \"group\":\"channel1\", \"exclusive\":true }");
	}
	labelChannel1 = mParams->addLabel("1", "{ \"width\":48 }");

	// Textures index channel 2
	mParams->addLabel("iChan2", "{ \"width\":64, \"clear\":false }");
	// Button Group for iChannel2
	for (int i = 1; i < mTextures->getTextureCount(); i++)
	{
		buttonChannel2[i] = mParams->addButton(toString(i), std::bind(&ChannelsPanel::setChannel2, this, i, std::placeholders::_1), "{ \"clear\":false, \"width\":9, \"stateless\":false, \"group\":\"channel2\", \"exclusive\":true }");
	}
	labelChannel2 = mParams->addLabel("2", "{ \"width\":48 }");

	// Meshes
	mParams->addLabel("mesh\ntype", "{ \"width\":64, \"clear\":false }");
	// Button Group for mesh
	for (int i = 0; i < 10; i++)
	{
		buttonMesh[i] = mParams->addButton(toString(i), std::bind(&ChannelsPanel::setCurrentMesh, this, i, std::placeholders::_1), "{ \"clear\":false, \"width\":9, \"stateless\":false, \"group\":\"mesh\", \"exclusive\":true }");
	}
	currentMeshLabel = mParams->addLabel("0", "{ \"width\":48 }");

	// camera control AND fbo preview
	sliderCamEyePointXY = mParams->addSlider2D("EyePointXY", &mParameterBag->mCamEyePointXY, "{ \"minX\":-10.0, \"maxX\":10.0, \"minY\":-10.0, \"maxY\":10.0, \"width\":" + toString(mParameterBag->mPreviewWidth) + " }");
	//sliderCamPosXY = mParams->addSlider2D("camPosXY", &mParameterBag->mCamPosXY, "{ \"minX\":-640.0, \"maxX\":640.0, \"minY\":-480.0, \"maxY\":480.0, \"width\":" + toString(mParameterBag->mPreviewWidth) + " }");

	// camera z
	sliderCamEyePointZ = mParams->addToggleSlider("EyePointZ", &mParameterBag->mCamEyePointZ, "a", std::bind(&ChannelsPanel::lockEyePointZ, this, std::placeholders::_1), "{ \"clear\":false, \"width\":" + toString(mParameterBag->mPreviewWidth - 39) + ", \"min\":" + toString(mParameterBag->minEyePointZ) + ", \"max\":" + toString(mParameterBag->maxEyePointZ) + " }", "{ \"width\":9, \"stateless\":false, \"group\":\"EyePointZ\", \"exclusive\":true, \"clear\":false }");
	mParams->addButton("t", std::bind(&ChannelsPanel::tempoEyePointZ, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"EyePointZ\", \"exclusive\":true, \"clear\":false }");
	mParams->addButton("x", std::bind(&ChannelsPanel::resetEyePointZ, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"EyePointZ\", \"exclusive\":true, \"pressed\":true, \"clear\":false }");
	mParams->addSlider("m", &mParameterBag->minEyePointZ, "{ \"min\":" + toString(mParameterBag->minEyePointZ) + ", \"max\":" + toString(mParameterBag->maxEyePointZ) + ", \"handleVisible\":false, \"width\":16, \"vertical\":true, \"clear\":false }");
	mParams->addSlider("M", &mParameterBag->maxEyePointZ, "{ \"min\":" + toString(mParameterBag->minEyePointZ) + ", \"max\":" + toString(mParameterBag->maxEyePointZ) + ", \"handleVisible\":false, \"width\":16, \"vertical\":true }");
}
void ChannelsPanel::lockEyePointZ(const bool &pressed)
{
	mParameterBag->autoEyePointZ = pressed;
}
void ChannelsPanel::tempoEyePointZ(const bool &pressed)
{
	mParameterBag->tEyePointZ = pressed;
	if (!pressed) resetEyePointZ(pressed);
}
void ChannelsPanel::resetEyePointZ(const bool &pressed)
{
	mParameterBag->autoEyePointZ = false;
	mParameterBag->tEyePointZ = false;
	mParameterBag->mCamEyePointZ = mParameterBag->defaultEyePointZ;
}
void ChannelsPanel::setChannel1(const int &aChannel, const bool &pressed)
{
	if (pressed) mParameterBag->iChannels[1] = aChannel;
}
void ChannelsPanel::setChannel2(const int &aChannel, const bool &pressed)
{
	if (pressed) mParameterBag->iChannels[2] = aChannel;
}

void ChannelsPanel::update()
{

	// animation	
	if (mVisible)
	{
		mParams->update();

		/*labelChannel1->setBackgroundTexture(mTextures->getTexture(mParameterBag->iChannels[1]));
		labelChannel2->setBackgroundTexture(mTextures->getTexture(mParameterBag->iChannels[2]));
		*/

		mParameterBag->mCamera.setEyePoint(Vec3f(mParameterBag->mCamEyePointXY.x, mParameterBag->mCamEyePointXY.y, mParameterBag->mCamEyePointZ));

		sliderCamEyePointZ->setName("eye point z " + toString(floor(mParameterBag->mCamEyePointZ)) + "\ " + toString(floor(mParameterBag->minEyePointZ)) + "-" + toString(floor(mParameterBag->maxEyePointZ)));
	}
}
void ChannelsPanel::draw()
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

void ChannelsPanel::resize()
{
	mParams->resize();
}
void ChannelsPanel::show()
{
	mVisible = true;
}
void ChannelsPanel::hide()
{
	mVisible = false;
}

void ChannelsPanel::setCurrentMesh(const int &aMesh, const bool &pressed)
{
	if (pressed)
	{
		mParameterBag->mMeshIndex = aMesh;
		currentMeshLabel->setName(toString(aMesh));
		if (aMesh == 0) mParameterBag->mCamPosXY = Vec2f(172.9, 338.2);
		if (aMesh == 1) mParameterBag->mCamPosXY = Vec2f(-155.6, -87.3);


	}
}
/*

void ChannelsPanel::setCurrentRenderShader(const int &aShaderIndex, const bool &pressed)
{
	if (pressed)
	{
		mShaders->setCurrentRenderShaderIndex(aShaderIndex);
		currentRenderShaderLabel->setName(toString(aShaderIndex));
	}
}
void ChannelsPanel::setCurrentPreviewShader(const int &aShaderIndex, const bool &pressed)
{
	if (pressed)
	{
		mShaders->setCurrentPreviewShaderIndex(aShaderIndex);
		currentPreviewShaderLabel->setName(toString(aShaderIndex));
	}
}*/
