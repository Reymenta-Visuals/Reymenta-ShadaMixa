#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Timeline.h"
// logger
#include "Logger.h"
// parameters
#include "ParameterBag.h"
// shaders
#include "Shaders.h"
// textures
#include "Textures.h"
// UI
#include "UI.h"
// transparent window
#include "dwmapi.h"
// warps
#include "WarpWrapper.h"
// window manager
#include "WindowMngr.h"
// OSC
#include "OSCWrapper.h"
// spout
#include "SpoutWrapper.h"
// preview
//#include "Preview.h"
// texture sequence
//#include "Sequence.h"
// render
//#include "Render.h"
// midi
#include "MidiIn.h"
#include "MidiMessage.h"
#include "MidiConstants.h"
// audio
#include "AudioWrapper.h"
// meshes
#include "Meshes.h"
// point sphere
#include "PointSphere.h"
// code editor
#include "CodeEditor.h"
// utilities
#include "Batchass.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace Reymenta;
using namespace MinimalUI;

using namespace ci;
using namespace ci::app;
using namespace std;

class ReymentaShadaMixaApp : public AppNative {
public:
	void prepareSettings(Settings *settings);
	void setup();
	void shutdown();
	void update();

	void resize();

	void mouseMove(MouseEvent event);
	void mouseDown(MouseEvent event);
	void mouseDrag(MouseEvent event);
	void mouseUp(MouseEvent event);
	void mouseWheel(MouseEvent event);

	void keyDown(KeyEvent event);
	void keyUp(KeyEvent event);
	void fileDrop(FileDropEvent event);
	void updateWindowTitle();
	void save();

private:
	// windows
	WindowRef					mMainWindow;
	void						windowManagement();
	void						getWindowsResolution();
	void						drawMain();
	WindowRef					mCodeEditorWindow;
	bool						mIsShutDown;
	// ui
	WindowRef					mUIWindow;
	void						drawUI();
	// code editor
	void						drawCodeEditor();
	void						createCodeEditor();
	bool						runCmd(string cmd);
	string						mCmdError;
	CodeEditorRef				mCodeEditor;
	// render
	void						createRenderWindow();
	void						deleteRenderWindows();
	vector<WindowMngr>			allRenderWindows;
	void						drawRender();
	void						showCodeCallback();
	void						createUIWindow();
	// modes, should be the same as in ParameterBag
	//static const int			MODE_NORMAL = 0;
	static const int			MODE_MIX = 1;
	static const int			MODE_AUDIO = 2;
	static const int			MODE_WARP = 3;
	static const int			MODE_SPHERE = 4;
	static const int			MODE_MESH = 5;
	static const int			MODE_KINECT = 6;

	// windows to create, should be the same as in ParameterBag
	static const int			NONE = 0;
	static const int			RENDER_1 = 1;
	static const int			RENDER_DELETE = 5;
	static const int			MIDI_IN = 6;

	// Logger
	LoggerRef					log;

	// Shaders
	ShadersRef					mShaders;
	// Textures
	TexturesRef					mTextures;
	// minimalUI
	UIRef						mUI;
	// imGui
	//UserInterfaceRef			mUserInterface;
	// parameters
	ParameterBagRef				mParameterBag;
	// utils
	//UtilsRef					mUtils;
	// warps
	WarpWrapperRef				mWarpings;
	// osc
	OSCRef						mOSC;
	// util
	BatchassRef					mBatchass;
	// spout
	SpoutWrapperRef				mSpout;
	// Preview
	//PreviewRef					mPreview;
	// sequence
	//SequenceRef					mSequence;
	// render
	//RenderRef					mRender;
	// audio
	AudioWrapperRef				mAudio;
	// mesh helper
	MeshesRef					mMeshes;
	// point sphere
	PointSphereRef				mSphere;
	// timeline
	Anim<float>					mTimer;
	// midi
	midi::Input					mMidiIn1;
	midi::Input					mMidiIn2;
	void						setupMidi();
	//void						midiListener1(midi::Message msg);
	//void						midiListener2(midi::Message msg);
	void						processMidiMsg(midi::Message msg);
	int							midiPort1;
	string						midiPort1Name;
	int							midiPort2;
	string						midiPort2Name;
	bool						removeUI;
};
