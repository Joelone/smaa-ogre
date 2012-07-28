#include "SMAA_Application.h"
#include "ogre-framework/Window.h"

#include <OgreSceneManager.h>
#include <OgreViewport.h>
#include <OgreCamera.h>
#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreColourValue.h>
#include <OgreManualObject.h>

#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>

#include <OgreCompositorManager.h>
#include <OgreCompositorInstance.h>
#include <OgreCompositorChain.h>

#define AREATEX_WIDTH 160
#define AREATEX_HEIGHT 560
#define AREATEX_PITCH (AREATEX_WIDTH * 2)
#define AREATEX_SIZE (AREATEX_HEIGHT * AREATEX_PITCH)

#define SEARCHTEX_WIDTH 66
#define SEARCHTEX_HEIGHT 33
#define SEARCHTEX_PITCH SEARCHTEX_WIDTH
#define SEARCHTEX_SIZE (SEARCHTEX_HEIGHT * SEARCHTEX_PITCH)


using namespace Ogre;

Application::Application()
{

}

//------------------------------------------------------------------------------//

Application::~Application()
{
}

//------------------------------------------------------------------------------//

void Application::destroyScene()
{
	CompositorManager::getSingleton().removeCompositorChain(mViewport);
}

void Application::createScene()
{
	mSceneMgr = Root::getSingleton().createSceneManager(ST_GENERIC);
	
	mCamera = mSceneMgr->createCamera("Camera");

	// load area & search textures

    ResourceGroupManager::getSingleton().addResourceLocation ("../media", "FileSystem");
    ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

/*

	unsigned char* buffer = 0;
	int length = 0;
	FILE* f = 0;

	buffer = new unsigned char[1024 * 1024];
	f = fopen("../media/smaa_area.raw", "rb");
	fread(buffer, AREATEX_WIDTH * AREATEX_HEIGHT * 2, 1, f);
	fclose(f);


	Ogre::DataStreamPtr stream = 
	Ogre::ResourceGroupManager::getSingleton().openResource(
		"smaa_area.raw", "General");

	Ogre::Image img;
	img.loadRawData(stream, AREATEX_WIDTH, AREATEX_HEIGHT, PF_RG8);
	
	TexturePtr areaTexture = TextureManager::getSingleton().createManual ("SMAA_AreaTexture", "General", TEX_TYPE_2D, AREATEX_WIDTH, AREATEX_HEIGHT, 0, PF_RG8);
	areaTexture->loadImage(img);



	std::ifstream inp2;
	inp2.open("../media/smaa_search.raw", std::ios::in | std::ios::binary);
	DataStreamPtr stream2(OGRE_NEW Ogre::FileStreamDataStream("../media/smaa_search.raw", &inp2, false));
	Image searchImage;
	searchImage.loadRawData(stream2, SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT, PF_R8);
	TexturePtr searchTexture = TextureManager::getSingleton().createManual ("SMAA_SearchTexture", "General", TEX_TYPE_2D, SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT, 0, PF_R8);
	searchTexture->loadImage(searchImage);
*/


	ResourceGroupManager::getSingleton().loadResourceGroup("General");

	// render a triangle

    MaterialPtr m = MaterialManager::getSingleton().getByName("BaseWhiteNoLighting");

	ManualObject* manual = mSceneMgr->createManualObject("manual");
	 
	// Use identity view/projection matrices
	manual->setUseIdentityProjection(true);
	manual->setUseIdentityView(true);
	 
	manual->begin("BaseWhiteNoLighting", RenderOperation::OT_TRIANGLE_LIST);

    manual->position(-0.5, -0.5, 0.0);
    manual->position( 0.5, -0.5, 0.0);
    manual->position( 0,  0.5, 0.0);
		 
	manual->end();
	 
	// Use infinite AAB to always stay visible
	AxisAlignedBox aabInf;
	aabInf.setInfinite();
	manual->setBoundingBox(aabInf);
	 
	manual->setRenderQueueGroup(RENDER_QUEUE_MAIN);
	 
	// Attach to scene
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);


	onRenderWindowRecreated();


}

//------------------------------------------------------------------------------//

void Application::onRenderWindowRecreated()
{
	mViewport = mWindow->mRenderWindow->addViewport(mCamera);
	mViewport->setBackgroundColour(ColourValue(0, 0, 0, 1));

	mCamera->setAspectRatio( float(mViewport->getActualWidth()) / float(mViewport->getActualHeight()));

	CompositorManager::getSingleton().addCompositor(mViewport, "SMAA");
	CompositorManager::getSingleton().setCompositorEnabled(mViewport, "SMAA", true);


	/// rendertextures debug

    //return;
	
	// init overlay elements
	OverlayManager& mgr = OverlayManager::getSingleton();
    Overlay* overlay;
	// destroy if already exists
	if (overlay = mgr.getByName("DebugOverlay"))
		mgr.destroy(overlay);
	overlay = mgr.create("DebugOverlay");
	Ogre::CompositorInstance  *compositor= CompositorManager::getSingleton().getCompositorChain(mViewport)->getCompositor("SMAA");
	for (int i=0; i<1; ++i)
	{
		// Set up a debug panel
		if (MaterialManager::getSingleton().resourceExists("Ogre/DebugTexture" + StringConverter::toString(i)))
			MaterialManager::getSingleton().remove("Ogre/DebugTexture" + StringConverter::toString(i));
		MaterialPtr debugMat = MaterialManager::getSingleton().create(
			"Ogre/DebugTexture" + StringConverter::toString(i), 
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		debugMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);

		TexturePtr depthTexture;

        depthTexture = compositor->getTextureInstance("edgeTex",0);


        //depthTexture = compositor->getTextureInstance("blendTex",0);

		//depthTexture = compositor->getTextureInstance("neighborhoodTex",0);

        //depthTexture = TextureManager::getSingleton().getByName("SMAA_Area.dds");
		
		
		if(!depthTexture.isNull())
		{
			TextureUnitState *t = debugMat->getTechnique(0)->getPass(0)->createTextureUnitState(depthTexture->getName());
			t->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
		}
		OverlayContainer* debugPanel;
		// destroy container if exists
		try
		{
			if (debugPanel = 
				static_cast<OverlayContainer*>(
					mgr.getOverlayElement("Ogre/DebugTexPanel" + StringConverter::toString(i)
				)))
				mgr.destroyOverlayElement(debugPanel);
		}
		catch (Ogre::Exception&) {}
		debugPanel = (OverlayContainer*)
			(OverlayManager::getSingleton().createOverlayElement("Panel", "Ogre/DebugTexPanel" + StringConverter::toString(i)));
		debugPanel->_setPosition(0,0);
		debugPanel->_setDimensions(1,1);
		debugPanel->setMaterialName(debugMat->getName());
		debugPanel->show();
		overlay->add2D(debugPanel);
		overlay->show();
	}

}
