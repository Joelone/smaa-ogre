#include "BaseApplication.h"
#include "PathManager.h"
#include "Window.h"
#include "InputManager.h"

#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OIS/OIS.h>

using namespace Ogre;

//------------------------------------------------------------------------------//

BaseApplication::BaseApplication() :
	mWindow(0), mWidth(1024), mHeight(768)
{

}

//------------------------------------------------------------------------------//

BaseApplication::~BaseApplication()
{
	
}

//------------------------------------------------------------------------------//

void BaseApplication::run()
{	
	// Construct Ogre::Root
	// We handle our stuff manually, so don't want to use any of the files
	mRoot = new Root(
		/* plugins.cfg file*/	"",
		/* config file */ 		"", 
		/* log file */ 			""
	);
	
	// Set render system
	mRoot->loadPlugin(PathManager::ogre_plugin_dir + "/Plugin_CgProgramManager");

	mRoot->loadPlugin(PathManager::ogre_plugin_dir + "/RenderSystem_GL");
	RenderSystem* rs = mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
	mRoot->setRenderSystem(rs);

	// Fire up Ogre::Root
	mRoot->initialise(false);

    Ogre::Root::getSingleton().getRenderSystem ()->setConfigOption ("sRGB Gamma Conversion", "true");

	// Create a hidden background window to keep resources
	NameValuePairList params;
	params.insert(std::make_pair("hidden", "true"));
	RenderWindow *wnd = mRoot->createRenderWindow("InactiveHidden", 1, 1, false, &params);
	wnd->setActive(false);

	// Create input system
	mInputManager = new InputManager();

	// Create the application window
	mWindow = new Window();
	mWindow->mListener = static_cast<WindowEventListener*>(this);
	mWindow->mInputManager = mInputManager;
	mWindow->create();
		
	// Start the rendering loop
	createScene();
	mRoot->addFrameListener(this);
	mRoot->startRendering();

	destroyScene();
	
	// Shutdown
	delete mWindow;
	delete mInputManager;
	mRoot->removeFrameListener(this);
	delete mRoot;
}

//------------------------------------------------------------------------------//

void BaseApplication::windowResized(RenderWindow* rw)
{
	
}

//------------------------------------------------------------------------------//

void BaseApplication::windowClosed(RenderWindow* rw)
{
	mShutdown = true;
}

//------------------------------------------------------------------------------//

void BaseApplication::recreateWindow()
{
	mWindow->recreate();
	onRenderWindowRecreated();
}

//------------------------------------------------------------------------------//

bool BaseApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	
	InputEvent* ev;
	
	mInputManager->process();
	
	while (true)
	{
		ev = mInputManager->pollEvents();
		
		if (ev == 0) break;
				
		if (ev->eventType == ET_Keyboard)
		{
			KeyEvent* kev = static_cast<KeyEvent*>(ev);
			if (kev->keyCode == OIS::KC_ESCAPE)
				mShutdown = true;
			else if (kev->keyCode == OIS::KC_R)
				recreateWindow();
		}

		delete ev;
	}
	
	return !mShutdown;
}
