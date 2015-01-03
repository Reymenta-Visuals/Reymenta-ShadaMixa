#include "ReymentaShadaMixaApp.h"

void ReymentaShadaMixaApp::prepareSettings(Settings *settings)
{
	// instanciate the logger class
	log = Logger::create("reymentalog.txt");
	log->logTimedString("start");

	// instanciate the json wrapper class
	//mJson = JSONWrapper::create();

	// parameters
	mParameterBag = ParameterBag::create();

	settings->setWindowSize(mParameterBag->mMainWindowWidth, mParameterBag->mMainWindowHeight);
	// Setting an unrealistically high frame rate effectively
	// disables frame rate limiting
	//settings->setFrameRate(10000.0f);
	settings->setFrameRate(60.0f);
	settings->setResizable(true);
	settings->setWindowPos(Vec2i(mParameterBag->mMainWindowX, mParameterBag->mMainWindowY));
}

void ReymentaShadaMixaApp::setup()
{
	log->logTimedString("setup");

	mIsShutDown = false;
	removeUI = false;

	// instanciate the Shaders class, must not be in prepareSettings
	mShaders = Shaders::create(mParameterBag);

	// instanciate the textures class
	mTextures = Textures::create(mParameterBag, mShaders);

	// instanciate the audio class
	mAudio = AudioWrapper::create(mParameterBag, mTextures);
	// utils
	mBatchass = Batchass::create(mParameterBag);

	windowManagement();
	// instanciate the warp wrapper class
	mWarpings = WarpWrapper::create(mParameterBag, mTextures, mShaders);
	// Setup the MinimalUI user interface
	mUI = UI::create(mParameterBag, mShaders, mTextures, mMainWindow);
	mUI->setup();
	//createUIWindow();
	// Setup the imGui user interface
	//mUserInterface = UserInterface::create(mParameterBag, mShaders, mTextures, mUIWindow);
	//mUserInterface->setup();

	// instanciate the Preview class
	//mPreview = Preview::create(mParameterBag, mTextures);
	// instanciate the OSC class
	mOSC = OSC::create(mParameterBag);
	// instanciate the Sequence class
	//mSequence = Sequence::create(mParameterBag, mTextures);
	// instanciate the Meshes class
	mMeshes = Meshes::create(mParameterBag, mTextures);
	// instanciate the PointSphere class
	mSphere = PointSphere::create(mParameterBag, mTextures, mShaders);
	// instanciate the Render class
	//mRender = Render::create(mParameterBag, mTextures);
	// midi
	//setupMidi();
	mTimer = 0.0f;
	mCursorVisible = true;
	mUI->tapTempo(true);
}
void ReymentaShadaMixaApp::windowManagement()
{
	log->logTimedString("windowManagement");
	mBatchass->getWindowsResolution();
	// setup the main window and associated draw function
	mMainWindow = getWindow();
	mMainWindow->setTitle("Reymenta ShadaMixa");
	mMainWindow->connectDraw(&ReymentaShadaMixaApp::drawMain, this);
	mMainWindow->connectClose(&ReymentaShadaMixaApp::shutdown, this);
}
void ReymentaShadaMixaApp::shutdown()
{
	if (!mIsShutDown)
	{
		mIsShutDown = true;
		log->logTimedString("shutdown");
		deleteRenderWindows();
		// save warp settings
		mWarpings->save();
		// save params
		mParameterBag->save();
		mUI->shutdown();
		if (mMeshes->isSetup()) mMeshes->shutdown();
		//mUserInterface->shutdown();
		//mPreview->shutdown();
		mShaders->shutdownLoader();
		quit();
	}
}

void ReymentaShadaMixaApp::update()
{
	if (mParameterBag->mNewMode != mParameterBag->mMode) mUI->changeMode(mParameterBag->mNewMode);

	/*if (mParameterBag->isShaderDirty)
	{
	mParameterBag->isShaderDirty = false;
	mShaders->loadFragJson(mParameterBag->mShaderToLoad);
	}*/
	if (mParameterBag->iGreyScale)
	{
		mParameterBag->controlValues[1] = mParameterBag->controlValues[2] = mParameterBag->controlValues[3];
		mParameterBag->controlValues[5] = mParameterBag->controlValues[6] = mParameterBag->controlValues[7];
	}

	mParameterBag->iChannelTime[0] = getElapsedSeconds();
	mParameterBag->iChannelTime[1] = getElapsedSeconds() - 1;
	mParameterBag->iChannelTime[3] = getElapsedSeconds() - 2;
	mParameterBag->iChannelTime[4] = getElapsedSeconds() - 3;
	//
	if (mParameterBag->mUseTimeWithTempo)
	{
		mParameterBag->iGlobalTime = mParameterBag->iTempoTime*mParameterBag->iTimeFactor;
	}
	else
	{
		mParameterBag->iGlobalTime = getElapsedSeconds();
	}

	mParameterBag->iFps = getAverageFps();
	updateWindowTitle();

	switch (mParameterBag->mMode)
	{
	case MODE_SPHERE:
		mSphere->update();
		break;
	case MODE_MESH:
		if (mMeshes->isSetup()) mMeshes->update();
		break;
	default:
		break;
	}
	if (!removeUI) mUI->update();
	mTextures->update();
	mOSC->update();
	mAudio->update();
	mShaders->update();
	if (mParameterBag->mWindowToCreate > 0)
	{
		// try to create the window only once
		int windowToCreate = mParameterBag->mWindowToCreate;
		mParameterBag->mWindowToCreate = NONE;
		switch (windowToCreate)
		{
		case RENDER_1:
			createRenderWindow();
			break;
		case RENDER_DELETE:
			deleteRenderWindows();
			break;
			/*case EDITOR_WINDOW:
			showCodeCallback();
			break;*/
		case MIDI_IN:
			setupMidi();
			break;
		}
	}
}

void ReymentaShadaMixaApp::drawCodeEditor()
{
	gl::clear(ColorAf(0.0f, 0.0f, 0.0f, 0.0f));
}
void ReymentaShadaMixaApp::showCodeCallback()
{
	if (mCodeEditorWindow == NULL)
	{
		mBatchass->getWindowsResolution();
		mParameterBag->mCodeEditorHeight = mParameterBag->mRenderHeight;
		//mCodeEditorWindow = createWindow(Window::Format().size(mParameterBag->mCodeEditorWidth, mParameterBag->mCodeEditorHeight));
		mCodeEditorWindow = createWindow(Window::Format().size(mMainWindow->getWidth(), mMainWindow->getHeight() - 150));
		// put it rightmost without scrollbar
		mParameterBag->mRenderCodeEditorXY.x = mParameterBag->mRenderX + mParameterBag->mRenderWidth - mParameterBag->mCodeEditorWidth + 20;
		//mCodeEditorWindow->setPos(mParameterBag->mRenderCodeEditorXY);
		mCodeEditorWindow->setPos(mMainWindow->getPos() + Vec2i(0, 100));
		//mCodeEditorWindow->setBorderless();
		mCodeEditorWindow->setAlwaysOnTop();
		mCodeEditorWindow->connectDraw(&ReymentaShadaMixaApp::drawCodeEditor, this);
		createCodeEditor();

		HWND hWnd = (HWND)mCodeEditorWindow->getNative();

		HRESULT hr = S_OK;
		// Create and populate the Blur Behind structure
		DWM_BLURBEHIND bb = { 0 };

		// Enable Blur Behind and apply to the entire client area
		bb.dwFlags = DWM_BB_ENABLE;
		bb.fEnable = true;
		bb.hRgnBlur = NULL;

		// Apply Blur Behind
		hr = DwmEnableBlurBehindWindow(hWnd, &bb);
		if (SUCCEEDED(hr))
		{
			HRESULT hr = S_OK;
			// Set the margins, extending the bottom margin.
			MARGINS margins = { -1 };
			// Extend the frame on the bottom of the client area.
			hr = DwmExtendFrameIntoClientArea(hWnd, &margins);
		}
		// connect a lambda unique to this window which fires on close
		/*int uniqueId = getNumWindows();
		mCodeEditorWindow->getSignalClose().connect(
		[uniqueId, this] { shutdown(); this->console() << "You closed window #" << uniqueId << std::endl; }
		);*/
	}

}
void ReymentaShadaMixaApp::drawUI()
{
	gl::clear(ColorAf(0.0f, 0.0f, 0.0f, 0.0f));
}

void ReymentaShadaMixaApp::createUIWindow()
{
	if (mUIWindow == NULL)
	{
		mBatchass->getWindowsResolution();
		mParameterBag->mCodeEditorHeight = mParameterBag->mRenderHeight;
		mUIWindow = createWindow(Window::Format().size(mMainWindow->getWidth(), mMainWindow->getHeight() - 150));
		// put it rightmost without scrollbar
		mParameterBag->mRenderCodeEditorXY.x = mParameterBag->mRenderX + mParameterBag->mRenderWidth - mParameterBag->mCodeEditorWidth + 20;
		//mCodeEditorWindow->setPos(mParameterBag->mRenderCodeEditorXY);
		mUIWindow->setPos(mMainWindow->getPos() + Vec2i(0, 100));
		//mCodeEditorWindow->setBorderless();
		mUIWindow->setAlwaysOnTop();
		mUIWindow->connectDraw(&ReymentaShadaMixaApp::drawUI, this);
		createCodeEditor();
		//mUserInterface->mCodePanel->mCodeEditor->connectWindow(mCodeEditorWindow);
		HWND hWnd = (HWND)mUIWindow->getNative();

		HRESULT hr = S_OK;
		// Create and populate the Blur Behind structure
		DWM_BLURBEHIND bb = { 0 };

		// Enable Blur Behind and apply to the entire client area
		bb.dwFlags = DWM_BB_ENABLE;
		bb.fEnable = true;
		bb.hRgnBlur = NULL;

		// Apply Blur Behind
		hr = DwmEnableBlurBehindWindow(hWnd, &bb);
		if (SUCCEEDED(hr))
		{
			HRESULT hr = S_OK;
			// Set the margins, extending the bottom margin.
			MARGINS margins = { -1 };
			// Extend the frame on the bottom of the client area.
			hr = DwmExtendFrameIntoClientArea(hWnd, &margins);
		}
	}
}
void ReymentaShadaMixaApp::createCodeEditor()
{
	mCodeEditor = CodeEditor::create(list_of<string>("shaders/current.frag")("shaders/current.cmd").convert_to_container<vector<fs::path>>(), CodeEditor::Settings().window(mCodeEditorWindow).autoSave().codeCompletion());
	//mCodeEditor = CodeEditor::create(list_of<string>("shaders/current.frag")("shaders/current.cmd").convert_to_container<vector<fs::path>>(), CodeEditor::Settings().autoSave().codeCompletion());

	mCodeEditor->registerCodeChanged("shaders/current.frag", [this](const string& frag)
	{
		if (mShaders->setFragString(frag))
		{
			mCodeEditor->clearErrors();
		}
		else
		{
			mCodeEditor->setError("current.frag: " + mShaders->getFragError());
			mParameterBag->InfoMsg = mShaders->getFragError();
		}
	});
	mCodeEditor->registerCodeChanged("shaders/commands.txt", [this](const string& cmd)
	{
		if (runCmd(cmd))
		{
			mCodeEditor->clearErrors();
		}
		else
		{
			mCodeEditor->setError("commands.txt: " + mCmdError);
		}
	});
	mCodeEditor->setTheme("dark");
	mCodeEditor->setOpacity(0.5f);
	mCodeEditor->enableLineWrapping(false);
	//mCodeEditor->connectWindow( mCodeEditorWindow );	
}
bool ReymentaShadaMixaApp::runCmd(string cmd)
{
	unsigned found = cmd.find_last_of("\n");
	string name = cmd.substr(0, found - 1);// .c_str());.substr(12, 1)
	unsigned before = name.find_last_of("\n");
	string command = cmd.substr(found + 1);
	mCmdError = command;
	return true;
}
void ReymentaShadaMixaApp::resize()
{
	mWarpings->resize();
}

void ReymentaShadaMixaApp::mouseMove(MouseEvent event)
{
	if (mParameterBag->mMode == MODE_WARP) mWarpings->mouseMove(event);
}

void ReymentaShadaMixaApp::mouseDown(MouseEvent event)
{
	if (mParameterBag->mMode == MODE_WARP) mWarpings->mouseDown(event);
	if (mParameterBag->mMode == MODE_MESH) mMeshes->mouseDown(event);
}

void ReymentaShadaMixaApp::mouseDrag(MouseEvent event)
{
	if (mParameterBag->mMode == MODE_WARP) mWarpings->mouseDrag(event);
	if (mParameterBag->mMode == MODE_MESH) mMeshes->mouseDrag(event);
}

void ReymentaShadaMixaApp::mouseUp(MouseEvent event)
{
	if (mParameterBag->mMode == MODE_WARP) mWarpings->mouseUp(event);
}
void ReymentaShadaMixaApp::mouseWheel(MouseEvent event)
{
	if (mParameterBag->mMode == MODE_MESH) mMeshes->mouseWheel(event);
}

void ReymentaShadaMixaApp::keyDown(KeyEvent event)
{
	string info;
	info = "shift:" + toString(event.isShiftDown()) + " alt:" + toString(event.isAltDown()) + " ctrl:" + toString(event.isControlDown()) + " accel:" + toString(event.isAccelDown());
	mParameterBag->InfoMsg = info;
	// ignore keyboard input if a modifier key is being held down
	//if (event.isShiftDown() || event.isAltDown() || event.isControlDown() || event.isMetaDown() || event.isAccelDown())
	//	return;

	if (event.isAltDown())
	{
		switch (event.getCode())
		{
			/*case ci::app::KeyEvent::KEY_n:
			mParameterBag->mMode = MODE_NORMAL;
			break;*/
		case ci::app::KeyEvent::KEY_a:
			mParameterBag->mMode = MODE_AUDIO;
			break;
		case ci::app::KeyEvent::KEY_s:
			mParameterBag->mMode = MODE_SPHERE;
			break;
		case ci::app::KeyEvent::KEY_w:
			mParameterBag->mMode = MODE_WARP;
			break;
		case ci::app::KeyEvent::KEY_m:
			mParameterBag->mMode = MODE_MESH;
			break;
		case ci::app::KeyEvent::KEY_l:
			if (mCodeEditorWindow) mCodeEditorWindow->setPos(mMainWindow->getPos() + Vec2i(0, 100));
			break;
		case ci::app::KeyEvent::KEY_r:
			if (mCodeEditorWindow) mCodeEditorWindow->setPos(mParameterBag->mRenderCodeEditorXY);
			break;
		case ci::app::KeyEvent::KEY_u:
			mUI->toggleVisibility();
			break;
		case ci::app::KeyEvent::KEY_b:
			if (mCodeEditorWindow) mCodeEditorWindow->setBorderless(!mCodeEditorWindow->isBorderless());
			break;
		case ci::app::KeyEvent::KEY_o:
			mParameterBag->mOriginUpperLeft = !mParameterBag->mOriginUpperLeft;
			break;
		case ci::app::KeyEvent::KEY_g:
			mParameterBag->iGreyScale = !mParameterBag->iGreyScale;
			break;
		case ci::app::KeyEvent::KEY_p:
			mParameterBag->mPreviewEnabled = !mParameterBag->mPreviewEnabled;
			break;
		case ci::app::KeyEvent::KEY_v:
			mParameterBag->controlValues[48] = !mParameterBag->controlValues[48];
			break;
		case ci::app::KeyEvent::KEY_f:
			if (allRenderWindows.size() > 0) allRenderWindows[0].mWRef->setFullScreen(!allRenderWindows[0].mWRef->isFullScreen());
			break;
		case ci::app::KeyEvent::KEY_x:
			removeUI = !removeUI;
			break;
		case ci::app::KeyEvent::KEY_c:
			if (mCursorVisible)
			{
				hideCursor();
			}
			else
			{
				showCursor();
			}
			mCursorVisible = !mCursorVisible;
			break;
		default:
			break;
		}
	}

	if (mParameterBag->mMode == MODE_WARP) mWarpings->keyDown(event);
}

void ReymentaShadaMixaApp::keyUp(KeyEvent event)
{
	if (mParameterBag->mMode == MODE_WARP) mWarpings->keyUp(event);
}

void ReymentaShadaMixaApp::updateWindowTitle()
{
	getWindow()->setTitle("(" + toString(floor(getAverageFps())) + " fps) Reymenta ShadaMixa");
}

void ReymentaShadaMixaApp::fileDrop(FileDropEvent event)
{
	bool loaded = false;
	string ext = "";
	// use the last of the dropped files
	boost::filesystem::path mPath = event.getFile(event.getNumFiles() - 1);
	string mFile = mPath.string();
	if (mFile.find_last_of(".") != std::string::npos) ext = mFile.substr(mFile.find_last_of(".") + 1);
	//mParameterBag->currentSelectedIndex = (int)(event.getX() / 80);//76+margin mParameterBag->mPreviewWidth);
	log->logTimedString(mFile + " dropped, currentSelectedIndex:" + toString(mParameterBag->currentSelectedIndex) + " x: " + toString(event.getX()) + " mPreviewWidth: " + toString(mParameterBag->mPreviewWidth));

	if (ext == "wav" || ext == "mp3")
	{
		//do not try to load by other ways
		loaded = true;
		mAudio->loadWaveFile(mFile);

	}
	if (ext == "png" || ext == "jpg")
	{
		//do not try to load by other ways
		loaded = true;
		//mTextures->loadImageFile(mParameterBag->currentSelectedIndex, mFile);
		mTextures->loadImageFile(1, mFile);
	}
	/*if (!loaded && ext == "frag")
	{
	//do not try to load by other ways
	loaded = true;
	//mShaders->incrementPreviewIndex();

	if (mShaders->loadPixelFrag(mFile))
	{
	mParameterBag->controlValues[13] = 1.0f;
	timeline().apply(&mTimer, 1.0f, 1.0f).finishFn([&]{ save(); });
	}
	if (mCodeEditor) mCodeEditor->fileDrop(event);
	}*/
	if (!loaded && ext == "glsl")
	{
		//do not try to load by other ways
		loaded = true;
		//mShaders->incrementPreviewIndex();
		//mUserInterface->mLibraryPanel->addShader(mFile);
		if (mShaders->loadPixelFragmentShader(mFile))
		{
			mParameterBag->controlValues[13] = 1.0f;
			// send content via OSC
			fs::path fr = mFile;
			string name = "unknown";
			if (mFile.find_last_of("\\") != std::string::npos) name = mFile.substr(mFile.find_last_of("\\") + 1);
			if (fs::exists(fr))
			{
				std::string fs = loadString(loadFile(mFile));
				mOSC->sendOSCStringMessage("/fs", 0, fs, name);
			}
			// save thumb
			timeline().apply(&mTimer, 1.0f, 1.0f).finishFn([&]{ save(); });
		}
	}
	if (!loaded && ext == "fs")
	{
		//do not try to load by other ways
		loaded = true;
		//mShaders->incrementPreviewIndex();
		mShaders->loadFragmentShader(mPath);
		timeline().apply(&mTimer, 1.0f, 1.0f).finishFn([&]{ save(); });
	}
	if (!loaded && ext == "patchjson")
	{
		// try loading patch
		//do not try to load by other ways
		loaded = true;
		try
		{
			JsonTree patchjson;
			try
			{
				patchjson = JsonTree(loadFile(mFile));
				mParameterBag->mCurrentFilePath = mFile;
			}
			catch (cinder::JsonTree::Exception exception)
			{
				log->logTimedString("patchjsonparser exception " + mFile + ": " + exception.what());

			}
			//Assets
			int i = 1; // 0 is audio
			JsonTree jsons = patchjson.getChild("assets");
			for (JsonTree::ConstIter jsonElement = jsons.begin(); jsonElement != jsons.end(); ++jsonElement)
			{
				string jsonFileName = jsonElement->getChild("filename").getValue<string>();
				int channel = jsonElement->getChild("channel").getValue<int>();
				if (channel < mTextures->getTextureCount())
				{
					log->logTimedString("asset filename: " + jsonFileName);
					mTextures->setTexture(channel, jsonFileName);
				}
				i++;
			}

		}
		catch (...)
		{
			log->logTimedString("patchjson parsing error: " + mFile);
		}
	}
	if (!loaded && ext == "txt")
	{
		//do not try to load by other ways
		loaded = true;
		// try loading shader parts
		if (mShaders->loadTextFile(mFile))
		{

		}
	}

	if (!loaded && ext == "")
	{
		//do not try to load by other ways
		loaded = true;
		// try loading image sequence from dir
		//mTextures->createFromDir(mFile + "/");

	}
	mParameterBag->isUIDirty = true;
}

void ReymentaShadaMixaApp::createRenderWindow()
{
	deleteRenderWindows();
	mBatchass->getWindowsResolution();

	mParameterBag->iResolution.x = mParameterBag->mRenderWidth;
	mParameterBag->iResolution.y = mParameterBag->mRenderHeight;
	mParameterBag->mRenderResolution = Vec2i(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight);

	log->logTimedString("createRenderWindow, resolution:" + toString(mParameterBag->iResolution.x) + "x" + toString(mParameterBag->iResolution.y));

	string windowName = "render";

	WindowRef	mRenderWindow;
	mRenderWindow = createWindow(Window::Format().size(mParameterBag->iResolution.x, mParameterBag->iResolution.y));

	// create instance of the window and store in vector
	WindowMngr rWin = WindowMngr(windowName, mParameterBag->mRenderWidth, mParameterBag->mRenderHeight, mRenderWindow);
	allRenderWindows.push_back(rWin);

	mRenderWindow->setBorderless();
	mParameterBag->mRenderResoXY = Vec2f(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight);
	mRenderWindow->connectDraw(&ReymentaShadaMixaApp::drawRender, this);
	mParameterBag->mRenderPosXY = Vec2i(mParameterBag->mRenderX, mParameterBag->mRenderY);//20141214 was 0
	mRenderWindow->setPos(mParameterBag->mRenderX, mParameterBag->mRenderY);

}
void ReymentaShadaMixaApp::deleteRenderWindows()
{
	for (auto wRef : allRenderWindows) DestroyWindow((HWND)wRef.mWRef->getNative());
	allRenderWindows.clear();
}
void ReymentaShadaMixaApp::drawMain()
{
	// draw the fbos
	mTextures->draw();
	//if (mParameterBag->mUIRefresh < 1.0) mParameterBag->mUIRefresh = 1.0;
	//if (getElapsedFrames() % (int)(mParameterBag->mUIRefresh + 0.1) == 0)
	if (getElapsedFrames() % mParameterBag->mUIRefresh == 0)
	{
		gl::clear();
		gl::setViewport(getWindowBounds());
		//20140703 gl::setMatricesWindow(getWindowSize());
		gl::setMatricesWindow(mParameterBag->mFboWidth, mParameterBag->mFboHeight, true);// mParameterBag->mOriginUpperLeft);

		gl::color(ColorAf(1.0f, 1.0f, 1.0f, 1.0f));
		if (mParameterBag->mPreviewEnabled)
		{
			// select drawing mode 
			switch (mParameterBag->mMode)
			{
				/*case MODE_NORMAL:
				gl::setMatricesWindow(mParameterBag->mPreviewFboWidth, mParameterBag->mPreviewFboHeight, mParameterBag->mOriginUpperLeft);
				gl::draw(mTextures->getFboTexture(mParameterBag->mCurrentPreviewFboIndex));
				break;
				case MODE_MIX:
				gl::draw(mTextures->getFboTexture(mParameterBag->mMixFboIndex));
				break;*/
			case MODE_AUDIO:
				mAudio->draw();
				gl::draw(mTextures->getFboTexture(mParameterBag->mAudioFboIndex));
				break;
			case MODE_WARP:
				mWarpings->draw();
				break;
			case MODE_SPHERE:
				mSphere->draw();
				gl::draw(mTextures->getFboTexture(mParameterBag->mSphereFboIndex));
				break;
			case MODE_MESH:
				if (mMeshes->isSetup())
				{
					mMeshes->draw();
					gl::draw(mTextures->getFboTexture(mParameterBag->mMeshFboIndex));
				}
				break;
			case MODE_KINECT:
				for (int i = 0; i < 20; i++)
				{
					gl::drawLine(Vec2f(mOSC->skeleton[i].x, mOSC->skeleton[i].y), Vec2f(mOSC->skeleton[i].z, mOSC->skeleton[i].w));
					gl::drawSolidCircle(Vec2f(mOSC->skeleton[i].x, mOSC->skeleton[i].y), 5.0f, 16);
				}
				break;
			default:
				gl::draw(mTextures->getFboTexture(mParameterBag->mMixFboIndex));
				break;
			}
		}
	}
	else
	{
		if (mParameterBag->mMode == MODE_MESH){ if (mMeshes->isSetup()) mMeshes->draw(); }
	}
	if (!removeUI) mUI->draw();
	//mUserInterface->draw();

	gl::disableAlphaBlending();
}
void ReymentaShadaMixaApp::drawRender()
{
	// clear
	gl::clear();
	// shaders			
	gl::setViewport(getWindowBounds());
	gl::enableAlphaBlending();
	//20140703 gl::setMatricesWindow(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight, mParameterBag->mOriginUpperLeft);//NEW 20140620, needed?
	gl::setMatricesWindow(mParameterBag->mFboWidth, mParameterBag->mFboHeight, true);// mParameterBag->mOriginUpperLeft);
	switch (mParameterBag->mMode)
	{
	case MODE_AUDIO:
		gl::draw(mTextures->getFboTexture(mParameterBag->mAudioFboIndex));
		break;
	case MODE_WARP:
		mWarpings->draw();
		break;
	case MODE_SPHERE:
		gl::draw(mTextures->getFboTexture(mParameterBag->mSphereFboIndex));
		break;
	case MODE_MESH:
		gl::draw(mTextures->getFboTexture(mParameterBag->mMeshFboIndex));
		break;
	default:
		gl::draw(mTextures->getFboTexture(mParameterBag->mMixFboIndex));
		break;
	}

	gl::disableAlphaBlending();
}
void ReymentaShadaMixaApp::save()
{
	string filename = mShaders->getMiddleFragFileName() + ".png";
	//string fpsFilename = ci::toString((int)getAverageFps()) + "-fps-" + mShaders->getMiddleFragFileName() + ".json";
	try
	{
		/*if (mParameterBag->iDebug)
		{
		JsonTree node = JsonTree();
		node.pushBack(JsonTree("RenderResolution width", mParameterBag->mRenderResolution.x));
		node.pushBack(JsonTree("RenderResolution height", mParameterBag->mRenderResolution.y));
		fs::path localFile = getAssetPath("") / "fps" / fpsFilename;
		node.write(localFile);
		log->logTimedString("saved:" + fpsFilename);
		Area area = Area(0, mParameterBag->mMainWindowHeight, mParameterBag->mPreviewWidth, mParameterBag->mMainWindowHeight - mParameterBag->mPreviewHeight);
		writeImage(getAssetPath("") / "thumbs" / filename, copyWindowSurface(area));*/
		writeImage(getAssetPath("") / "thumbs" / filename, mTextures->getFboTexture(mParameterBag->mCurrentPreviewFboIndex));
		log->logTimedString("saved:" + filename);
		//}
	}
	catch (const std::exception &e)
	{
		log->logTimedString("unable to save:" + filename + string(e.what()));
	}
}
// midi
void ReymentaShadaMixaApp::setupMidi()
{
	// midi
	midiPort1 = 0;
	midiPort2 = 0;
	if (mMidiIn1.getNumPorts() > 0)
	{
		mMidiIn1.listPorts();
		for (int i = 0; i < mMidiIn1.getNumPorts(); i++)
		{
			log->logTimedString("port: " + mMidiIn1.mPortNames[i]);
			size_t loopbeFound = mMidiIn1.mPortNames[i].find("Loop");
			if (loopbeFound != string::npos)
			{
				midiPort1 = i;
				midiPort1Name = mMidiIn1.mPortNames[i];
			}
		}
		mMidiIn1.openPort(midiPort1);
		mMidiIn1.midiSignal.connect(boost::bind(&ReymentaShadaMixaApp::processMidiMsg, this, boost::arg<1>::arg()));
		log->logTimedString("Opening MIDIin1 port " + toString(midiPort1) + " " + midiPort1Name);
	}
	else
	{
		log->logTimedString("No MIDIin1 Ports found!!!!");
	}
	if (mMidiIn2.getNumPorts() > 0)
	{
		mMidiIn2.listPorts();
		for (int i = 0; i < mMidiIn2.getNumPorts(); i++)
		{
			log->logTimedString("port: " + mMidiIn2.mPortNames[i]);
			size_t nanoFound = mMidiIn2.mPortNames[i].find("nano");
			if (nanoFound != string::npos)
			{
				midiPort2 = i;
				midiPort2Name = mMidiIn2.mPortNames[i];
			}
		}
		if (midiPort2 != midiPort1)
		{
			mMidiIn2.openPort(midiPort2);

			mMidiIn2.midiSignal.connect(boost::bind(&ReymentaShadaMixaApp::processMidiMsg, this, boost::arg<1>::arg()));

			log->logTimedString("Opening MIDIin2 port " + toString(midiPort2) + " " + midiPort2Name);
		}
		else
		{
			log->logTimedString("Already opened MIDIin2 port " + toString(midiPort2) + " " + midiPort2Name);
		}
	}
	else
	{
		log->logTimedString("No MIDIin2 Ports found!!!!");
	}
}
//void ReymentaShadaMixaApp::midiListener1(midi::Message msg)
//{
//	processMidiMsg(msg);
//}
//void ReymentaShadaMixaApp::midiListener2(midi::Message msg)
//{
//	processMidiMsg(msg);
//}
void ReymentaShadaMixaApp::processMidiMsg(midi::Message msg)
{
	stringstream ssName;
	int name;
	int newValue;
	string controlName;
	//BYTE b;
	string fileName;
	fs::path fragFile;
	string fullPath;
	newValue = msg.value;
	float normalizedValue;

	switch (msg.status)
	{
	case MIDI_CONTROL_CHANGE:
		name = msg.control;
		ssName << name;
		controlName = ssName.str();
		normalizedValue = lmap<float>(newValue, 0.0, 127.0, 0.0, 1.0);
		//log->logTimedString("MIDI_CONTROL_CHANGE " + controlName + " newvalue: " + toString(newValue) + " norm:" + toString(normalizedValue));

		if (name < mParameterBag->controlValues.size() && normalizedValue != mParameterBag->controlValues[name])
		{
			mParameterBag->controlValues[name] = normalizedValue;
			//48 invert
			mParameterBag->OSCMsg = "MIDI_CC " + controlName + " new: " + toString(newValue) + " norm:" + toString(normalizedValue);
			switch (name)
			{
			case 41:
				mUI->toggleVisibility();
				break;

			default:
				// rotary
				//if (name > 10 && name < 19) mOSC->rotaryChange(name, normalizedValue);
				// toggle button
				//if (name > 20 && name < 48) mOSC->toggleChange(name, newValue);
				break;
			}
		}
		break;
	case MIDI_NOTE_ON:
		name = msg.pitch;
		newValue = msg.velocity;
		normalizedValue = lmap<float>(newValue, 0.0, 127.0, 0.0, 1.0);
		ssName << name;
		controlName = ssName.str();
		log->logTimedString("MIDI_NOTE_ON " + controlName + " newvalue: " + toString(newValue) + " norm:" + toString(normalizedValue));
		if (name == 127) mUI->tapTempo(true);
		if (name > 9 && name < 16)
		{

		}
		if (name > 0 && name < 9)
		{
			// send to OSC for resolume
			mOSC->sendOSCIntMessage("/layer1/clip" + controlName + "/connect", 1, 1, 1);
		}
		//if ( name > 20 && name < 40 )
		/*if (name > 14 && name < 22)
		{
		fileName = toString(name) + ".fragjson";
		fragFile = getAssetPath("") / "shaders" / "fragjson" / fileName;
		fullPath = fragFile.string();
		//21:A-1//48:C2
		// KO mShaders->loadFragJson( fullPath ); cannot compile the shader file!
		mParameterBag->mShaderToLoad = fullPath;
		mParameterBag->isShaderDirty = true;
		}*/
		//b = msg.byteOne;
		//keybd_event( mJson->keyboardValues[ b ], NULL, NULL, NULL );
		break;
	default:
		break;
	}
}
CINDER_APP_NATIVE(ReymentaShadaMixaApp, RendererGl)
