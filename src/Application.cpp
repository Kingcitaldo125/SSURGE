#include <stdafx.h>
#include <utility.h>
#include <application.h>
#include <GameObject.h>
#include <SoundComponent.h>
#include <ReverbZoneComponent.h>
#include <MeshComponent.h>		// temporary
#include <Tuple.h>
#include <ParticleEmitter.h>
#include <MovableTextOverlay.h>

#define USE_TEST_GAME 0                   // 0 if doing normal testing, 1 if testing the python game.

// Template specialization to initialize the application's singleton pointer
template<>
ssurge::Application * ssurge::Singleton<ssurge::Application>::msSingleton = nullptr;


ssurge::Application::Application() : mRoot(NULL), mInputManager(NULL), mSoundManager(NULL),
mWindow(NULL), mLogManager(NULL), mSceneManager(NULL), mGameObjectManager(NULL), mFullscreenEffectManager(NULL),
mOverlaySystem(NULL)
{
	// Put any pre-startup code here (that can't be initialized as above)

	// Set the application state
	mState = ApplicationState::created;
}



ssurge::Application::~Application()
{
	// This was probably already called by the run method, but the method
	// is safe to re-call.
	shutdown();
}


	
int ssurge::Application::initialize(std::string window_title, bool local, bool server, bool strict)
{
	// Create the Ogre root
#ifdef _DEBUG
	mRoot = new Ogre::Root("plugins_d.cfg");
#else
	mRoot = new Ogre::Root("plugins.cfg");
#endif

	// Create the LogManager.  Note: THIS is the LogManager singleton.
	// When we use LOG_MANAGER->, we're really de-referencing this object.
	mLogManager = new LogManager("ssurge_log.txt");
	if (mLogManager == NULL)
		return 1;
#ifdef _DEBUG
	mLogManager->setLogMask();
#else
	mLogManager->setLogMask(LL_ERROR);
#endif

	// According to the forums (http://www.ogre3d.org/tikiwiki/Manual+Resource+Loading#Overlay), this must be done before calling
	// root->initialize()
	mOverlaySystem = new Ogre::OverlaySystem();
	if (mOverlaySystem == NULL)
		return 2;
	LOG_MANAGER->log("Created Overlay System", LL_NORMAL);


	// Show the configuration dialog box
	if (!mRoot->showConfigDialog())
		return 2;			// The user cancelled in the dialog box.
	LOG_MANAGER->log("Successfully created Ogre Root", LL_NORMAL);

	// Create the Ogre Window
	mWindow = mRoot->initialise(true, window_title);
	if (!mWindow)
		return 3;
	LOG_MANAGER->log("Successfully created render window", LL_NORMAL);

	// Create the input system
	mInputManager = new InputManager(mWindow, "..\\bindings.xml");
	if (!mInputManager)
		return 4;
	LOG_MANAGER->log("Successfully created Input System", LL_NORMAL);

	// Create the Ogre Scene Manager
	mSceneManager = mRoot->createSceneManager("OctreeSceneManager");
	if (!mSceneManager)
		return 5;
	LOG_MANAGER->log("Created Ogre Scene Manager");

	// Register the overlay system as a render queue listener
	mSceneManager->addRenderQueueListener(mOverlaySystem);

	// Create the physics manager
	mPhysicsManager = new PhysicsManager();
	if (!mPhysicsManager)
		return 6;
	LOG_MANAGER->log("Created PhysicsManager");

	// Create the sound manager
	mSoundManager = new SoundManager();
	if (!mSoundManager)
		return 7;
	LOG_MANAGER->log("Created SoundManager");

	// Idenitify our resource locations and load those resources.
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\Compositors", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\Fonts", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\Materials", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\Misc", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\Meshes", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\Particles", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\Scenes", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\Scripts", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\Shaders", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\Skeletons", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\Sounds", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\Textures", "FileSystem");

	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	LOG_MANAGER->log("Loaded all resources", LL_NORMAL);

	// Create the game object manager
	mGameObjectManager = new GameObjectManager();
	if (!mGameObjectManager)
		return 8;
	LOG_MANAGER->log("Created GameObjectManager");


	// Create the script manager
	mScriptManager = new ScriptManager();
	if (!mScriptManager)
		return 9;
	LOG_MANAGER->log("Created ScriptManager");

	// Load all python scripts 
	std::vector<std::string> s;
	ssurge::find_fileType(s, "*.py");
	for (std::string stf : s)
	{
		mScriptManager->loadScript("..\\media\\Scripts\\" + stf);
	}

	// Create the fullscreen effect manager (for overlay and compositor support currently).
	mFullscreenEffectManager = new FullscreenShaderManager();
	if (!mFullscreenEffectManager)
		return 11;
	LOG_MANAGER->log("Created Fullscreen Effect Manager");


	// Create the GUI Manager
	mGUIManager = new GUIManager();
	if (!mGUIManager)
		return 12;
	LOG_MANAGER->log("Created GUI Manager");
	// Temporary...
	mGUIManager->createImageOverlay("Mask", 0.75, 0.0);
	int testText = mGUIManager->createTextOverlay("Testing text!", 0.0, 0.0);
	int removedText = mGUIManager->createTextOverlay("This shouldn't be here", 0.5, 0.5);
	mGUIManager->removeOverlay("Text" + removedText);
	mGUIManager->moveOverlay("Text" + testText, 0.5, 0.0);
	mGUIManager->logText("test log");



	// Create the network manager
	mNetworkManager = new NetworkManager(local,server,strict);
	if (!mNetworkManager)
		return 13;
	LOG_MANAGER->log("Created Network Manager");

	// Call our createScene method
	if (createScene())
	{
		LOG_MANAGER->log("Error creating scene", LL_ERROR);
		return 10;
	}
	LOG_MANAGER->log("Created scene", LL_NORMAL);


	
	mLuaScriptManager = new LuaScriptManager();
	if (!mLuaScriptManager)
		return 11;
	LOG_MANAGER->log("Created LuaScriptManager");

	Component::initializeComponentMaps();

	// We're done!  Set our state to initialized.
	mState = ApplicationState::initialized;
	return 0;
}



int ssurge::Application::shutdown()
{
	if (mState == ApplicationState::dead)
		return 0;		// Already cleaned up!

	// Destroy the network manager
	if (mNetworkManager)
		delete mNetworkManager;
	mNetworkManager = NULL;

	// Destroy the GUI manager
	if (mGUIManager)
		delete mGUIManager;
	mGUIManager = NULL;

	// Destroy the full-screen effect manager
	if (mFullscreenEffectManager)
		delete mFullscreenEffectManager;
	mFullscreenEffectManager = NULL;

	// Destroy the ScriptManager
	if (mScriptManager)
		delete mScriptManager;
	mScriptManager = NULL;

	// Destroy the game object manager
	if (mGameObjectManager)
		delete mGameObjectManager;
	mGameObjectManager = NULL;

	// Destroy the OIS Input Objects
	if (mInputManager)
		delete mInputManager;
	mInputManager = NULL;
	LOG_MANAGER->log("Destroyed Input Manager", LL_NORMAL);

	// Destroy to overlay system
	if (mOverlaySystem)
		delete mOverlaySystem;
	mOverlaySystem = NULL;
	LOG_MANAGER->log("Destroyed Overlay System", LL_NORMAL);

	// Destroy the Ogre root (this should clean up any Ogre Objects)
	if (mRoot)
		delete mRoot;
	mRoot = NULL;
	mWindow = NULL;
	LOG_MANAGER->log("Destroyed Ogre root", LL_NORMAL);

	// Destroy the LogManager
	LOG_MANAGER->log("About to destroy LogManager...goodbye.", LL_NORMAL);
	if (mLogManager)
		delete mLogManager;
	mLogManager = NULL;

	Component::destroyComponentMaps();

	mState = ApplicationState::dead;
	return 0;
}



void ssurge::Application::setActiveCamera(Ogre::Camera * c)
{
	// If we don't have a viewport yet, create one.
	if (mWindow->getNumViewports() == 0)
	{
		Ogre::Viewport * viewport = mWindow->addViewport(c);
		viewport->setOverlaysEnabled(true);
		//viewport->setBackgroundColour(Ogre::ColourValue(0.3f, 0.3f, 0.3f));
	}
	else
		mWindow->getViewport(0)->setCamera(c);
	//std::string name = "Invert";
	//Ogre::CompositorManager::getSingleton().addCompositor(mWindow->getViewport(0), name);
	//Ogre::CompositorManager::getSingleton().setCompositorEnabled(mWindow->getViewport(0), name, true);
}



int ssurge::Application::run()
{
	if (mState != ApplicationState::initialized)
		return 1;

	mState = ApplicationState::running;
/*
//~~~~~~***** AUDIO TESTING CODE
	//Preload a sound
	SOUND_MANAGER->preloadSound("..\\media\\Sounds\\MF-3DAYS.S3M");
	
	//Set up first sound
	GameObject * go = GAME_OBJECT_MANAGER->createGameObject("sound", 69);
	go->setPosition(Ogre::Vector3::ZERO);
	SoundComponent * s1 = new SoundComponent(go, "..\\media\\Sounds\\MF-3DAYS.S3M", false);
	go->addComponent(s1);
	s1->playSound(1.0f, true, 3.0f);
	
	//Set up second sound
	go = GAME_OBJECT_MANAGER->createGameObject("sound2", 70);
	go->setPosition(Ogre::Vector3(0, 9, 3));
	SoundComponent * s2 = new SoundComponent(go, "..\\media\\Sounds\\gunshot3.wav", false);
	MeshComponent * m = new MeshComponent(go);
	m->loadMesh("Icosphere.mesh");
	go->addComponent(s2);
	s2->playSound(1.0f, true, 3.0f);
	
	//Set up reverb stuff
	SOUND_MANAGER->enableReverb();
	GameObject * rz1o = GAME_OBJECT_MANAGER->createGameObject("rz1", 69);
	GameObject * rz2o = GAME_OBJECT_MANAGER->createGameObject("rz2", 69);
	rz1o->setPosition(Ogre::Vector3(2, 7, 2));
	rz2o->setPosition(Ogre::Vector3(2, -7, 2));
	ReverbZoneComponent * rz1c = new ReverbZoneComponent(rz1o, 3.5f, 6.5f, 0.5f, 1500.0f);
	ReverbZoneComponent * rz2c = new ReverbZoneComponent(rz2o, 3.5f, 12.5f, 0.5f, 2500.0f);
	
	//Prep stuff 
	float orbAng = 0.0f;
	float orbSpeed = 0.5f;
//---------- END TESTING CODE
*/	

	mTimer.reset();


	while (mState == ApplicationState::running)
	{
		// Do updates
		double dt = mTimer.getMicroseconds() * 0.000001;
		mTimer.reset();
		if (!update(dt))
			break;


		///Call the Camera Component's update method.
		if(mCameraComponent)
			mCameraComponent->update(dt);

		// Update sound manager
		mSoundManager->update(dt);

/*
//~~~~~~***** AUDIO TESTING CODE 2: TEST HARDER
		// Turn gunshot sound on or off
		if (INPUT_MANAGER->isKeyDown(OIS::KC_1))
			s2->playSound(1.0f, true, 5.0f);
		if (INPUT_MANAGER->isKeyDown(OIS::KC_2))
			s2->stopSound();

		// Move orb around
		if (INPUT_MANAGER->isKeyDown(OIS::KC_3))
			orbSpeed -= 0.05f;
		if (INPUT_MANAGER->isKeyDown(OIS::KC_4))
			orbSpeed += 0.05f;
		orbAng = std::fmod(orbAng + orbSpeed * (float)dt, 6.28318530718f);
		go->setPosition(Ogre::Vector3(4.0f + 9.0f * std::sinf(orbAng), 1.0f + 9.0f * std::cosf(orbAng), 3.0f));

		// Enable / Disable chorus effect
		if (INPUT_MANAGER->isKeyDown(OIS::KC_5))
			s1->getEffectControl()->enableChorusSoundEffect();
		if (INPUT_MANAGER->isKeyDown(OIS::KC_6))
			s1->getEffectControl()->disableChorusSoundEffect();
//----------  END TESTING CODE
*/

		// Check input
		mInputManager->update(dt);
		if (mWindow->isClosed())						// true if the close button was pressed
			mState = ApplicationState::readyToQuit;
		if (mInputManager->isKeyDown(OIS::KC_ESCAPE))
			mState = ApplicationState::readyToQuit;

		// Draw
		mRoot->renderOneFrame(static_cast<float>(dt));
		
		// Not technically necessary, but makes this app play nicely with 
		// other processes that are running
		Ogre::WindowEventUtilities::messagePump();
	}

	// Trigger a system shutdown.
	shutdown();

	return 0;
}



int ssurge::Application::createScene()
{

#if USE_TEST_GAME
	mGameObjectManager->loadXML("..\\media\\Scenes\\python_main.py");
#else

	// Load the scene via an xml file
	if (mGameObjectManager)
		mGameObjectManager->loadXML("..\\media\\Scenes\\Lab17.scene", 10);

	GameObject * ninja = mGameObjectManager->findGameObject("Ninja");
	ssurge::MeshComponent * mc = static_cast<MeshComponent*>(ninja->getComponent(ssurge::ComponentType::MESH));
	mc->setDefaultAnim("Idle1");
	mc->addBoneMask("upper", { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 });
	mc->addBoneMask("lower", { 17,18,19,20,21,22,23,24,25,26,27 });
	SOUND_MANAGER->setListenerGameObject(ninja);
	//Need get camera refrence
	//MovableTextOverlay* testText = new MovableTextOverlay("Text", " Ninja ", ninja, ("Attrs1", cameraObject, "BlueHighway", 16, ColourValue::White, "RedTransparent"));

	// Code to log the shader versions supported by this graphics card...
	// Reference: http://www.ogre3d.org/forums/viewtopic.php?f=2&t=15587
	Ogre::GpuProgramManager::SyntaxCodes shader_names = Ogre::GpuProgramManager::getSingleton().getSupportedSyntax();
	Ogre::GpuProgramManager::SyntaxCodes::const_iterator shader_name_iter = shader_names.cbegin();
	LOG_MANAGER->log("Supported Shaders", LL_NORMAL);
	while (shader_name_iter != shader_names.cend())
	{
		std::string name = *shader_name_iter;
		LOG_MANAGER->log("\t" + name, LL_NORMAL);
		shader_name_iter++;
	}
	// ...end shader version debug information
	//Particle Testing (Daniel Bowen)
	GameObject * bigMonkey = mGameObjectManager->findGameObject("BigMonkey");
	ssurge::ParticleEmitter* p = new ssurge::ParticleEmitter(bigMonkey, "Space/Sun");

	// Compositor test
	//@JW: Gaussian is still a work-in-progress (see FullscreenShaderManager.cpp)
	std::vector<std::string> compNames; // = { "BW"/*, "Gaussian"*/ };
	for (unsigned int i = 0; i < compNames.size(); i++)
	{
		Ogre::CompositorInstance * inst = Ogre::CompositorManager::getSingleton().addCompositor(mWindow->getViewport(0), compNames[i]);
		if (inst)
			inst->setEnabled(true);
		else
			LOG_MANAGER->log("Unable to create compositor '" + compNames[i] + "'");
	}
	// ...end Compositor test

	

	GameObject * testGOBJ = GAME_OBJECT_MANAGER->findGameObject("Floor");
	MeshComponent * testMesh = static_cast<MeshComponent*>(testGOBJ->getComponent(ssurge::ComponentType::MESH));

	// start of billboard test
	Ogre::SceneNode* myNode = APPLICATION->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	Ogre::BillboardSet* mySet = APPLICATION->getSceneManager()->createBillboardSet("mySet");
	mySet->setMaterialName("Mask");
	mySet->setVisible(true);
	mySet->setUseAccurateFacing(true);
	for (int i = 0; i < 50; ++i)
	{
		Ogre::Billboard *board = mySet->createBillboard(Ogre::Vector3(0, 1, -i * 20), Ogre::ColourValue::Red);
		board->setDimensions(8, 1);
	}

	myNode->attachObject(mySet);


	////  end of test

	// Testing code for CG shader
	GameObject * shaderGO = GAME_OBJECT_MANAGER->createGameObject("monkeyhead2", 420);
	MeshComponent * shaderMesh = new MeshComponent(shaderGO);
	shaderMesh->loadMesh("BigMonkey.mesh");
	shaderGO->setPosition(-10, -5, 0);
	shaderMesh->setMaterial(0, "shader/test1");


	/*GameObject * extraHead = GAME_OBJECT_MANAGER->createGameObject("extrahead: 1",101);
	MeshComponent * extraMesh = new MeshComponent(extraHead);
	extraMesh->loadMesh("BigMonkey.mesh");
	extraHead->setPosition(0, 0, 60);
	ColliderComponent * extraColl = new ColliderComponent(extraHead, "capsule");
	RigidBodyComponent* extraRigid = new RigidBodyComponent(extraHead, extraColl->getShape(), 1);
	extraRigid->getRigidBody()->setRestitution(btScalar(1.5));
	
	GameObject * extraHead2 = GAME_OBJECT_MANAGER->createGameObject("extrahead: 2", 102);
	MeshComponent * extraMesh2 = new MeshComponent(extraHead2);
	extraMesh2->loadMesh("BigMonkey.mesh");
	extraHead2->setPosition(0, 0, 10);
	ColliderComponent * extraColl2 = new ColliderComponent(extraHead2, "cone");
	RigidBodyComponent* extraRigid2 = new RigidBodyComponent(extraHead2, extraColl2->getShape(), 1);
	extraRigid2->getRigidBody()->setRestitution(btScalar(1.5));

	GameObject * extraHead3 = GAME_OBJECT_MANAGER->createGameObject("extrahead: 3", 103);
	MeshComponent * extraMesh3 = new MeshComponent(extraHead3);
	extraMesh3->loadMesh("BigMonkey.mesh");
	extraHead3->setPosition(0, 0, 5);
	ColliderComponent * extraColl3 = new ColliderComponent(extraHead3, "cylinder");
	RigidBodyComponent* extraRigid3 = new RigidBodyComponent(extraHead3, extraColl3->getShape(), 1);
	extraRigid3->getRigidBody()->setRestitution(btScalar(1.5));

	GameObject * extraHead4 = GAME_OBJECT_MANAGER->createGameObject("extrahead: 4", 104);
	MeshComponent * extraMesh4 = new MeshComponent(extraHead4);
	extraMesh4->loadMesh("BigMonkey.mesh");
	extraHead4->setPosition(0, 0, 80);
	ColliderComponent * extraColl4 = new ColliderComponent(extraHead4, "box");
	RigidBodyComponent* extraRigid4 = new RigidBodyComponent(extraHead4, extraColl4->getShape(), 1);
	extraRigid4->getRigidBody()->setRestitution(btScalar(1.5));

	GameObject * extraHead5 = GAME_OBJECT_MANAGER->createGameObject("extrahead: 5", 101);
	MeshComponent * extraMesh5 = new MeshComponent(extraHead5);
	extraMesh5->loadMesh("BigMonkey.mesh");
	extraHead5->setPosition(0, 5, 60);
	ColliderComponent * extraColl5 = new ColliderComponent(extraHead5, "capsule");
	RigidBodyComponent* extraRigid5 = new RigidBodyComponent(extraHead5, extraColl5->getShape(), 1);
	extraRigid5->getRigidBody()->setRestitution(btScalar(1.5));

	GameObject * extraHead6 = GAME_OBJECT_MANAGER->createGameObject("extrahead: 6", 102);
	MeshComponent * extraMesh6 = new MeshComponent(extraHead6);
	extraMesh6->loadMesh("BigMonkey.mesh");
	extraHead6->setPosition(0, 5, 10);
	ColliderComponent * extraColl6 = new ColliderComponent(extraHead6, "cone");
	RigidBodyComponent* extraRigid6 = new RigidBodyComponent(extraHead6, extraColl6->getShape(), 1);
	extraRigid6->getRigidBody()->setRestitution(btScalar(1.5));

	GameObject * extraHead7 = GAME_OBJECT_MANAGER->createGameObject("extrahead: 7", 103);
	MeshComponent * extraMesh7 = new MeshComponent(extraHead7);
	extraMesh7->loadMesh("BigMonkey.mesh");
	extraHead7->setPosition(0, -5, 20);
	ColliderComponent * extraColl7 = new ColliderComponent(extraHead7, "cylinder");
	RigidBodyComponent* extraRigid7 = new RigidBodyComponent(extraHead7, extraColl7->getShape(), 1);
	extraRigid7->getRigidBody()->setRestitution(btScalar(1.5));

	GameObject * extraHead8 = GAME_OBJECT_MANAGER->createGameObject("extrahead: 8", 104);
	MeshComponent * extraMesh8 = new MeshComponent(extraHead8);
	extraMesh8->loadMesh("BigMonkey.mesh");
	extraHead8->setPosition(0, -10, 80);
	ColliderComponent * extraColl8 = new ColliderComponent(extraHead8, "box");
	RigidBodyComponent* extraRigid8 = new RigidBodyComponent(extraHead8, extraColl8->getShape(), 1);
	extraRigid8->setRestitution(0);
	extraRigid8->setFriction(1);

	//Possible code for dynamic shader loading (without material file)
	/*
	std::string customCasterMatVp =
		"void customCasterVp(float4 position : POSITION,\n"
		"out float4 oPosition : POSITION,\n"
		"uniform float4x4 worldViewProj)\n"
		"{\n"
		"   oPosition = mul(worldViewProj, position);\n"
		"}\n";
	std::string customCasterMatFp =
		"void customCasterFp(\n"
		"out float4 oColor : COLOR)\n"
		"{\n"
		"   oColor = float4(1,1,0,1); // just a test\n"
		"}\n";

	Ogre::HighLevelGpuProgramPtr vp = Ogre::HighLevelGpuProgramManager::getSingleton()
		.createProgram("CustomShadowCasterVp",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			"cg", Ogre::GPT_VERTEX_PROGRAM);
	vp->setSource(customCasterMatVp);
	vp->setParameter("profiles", "vs_1_1 arbvp1");
	vp->setParameter("entry_point", "customCasterVp");
	vp->load();

	Ogre::HighLevelGpuProgramPtr fp = Ogre::HighLevelGpuProgramManager::getSingleton()
		.createProgram("CustomShadowCasterFp",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			"cg", Ogre::GPT_FRAGMENT_PROGRAM);
	fp->setSource(customCasterMatFp);
	fp->setParameter("profiles", "ps_1_1 arbfp1");
	fp->setParameter("entry_point", "customCasterFp");
	fp->load();

	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("CustomShadowCaster",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::Pass* pass = mat->getTechnique(0)->getPass(0);
	pass->setVertexProgram("CustomShadowCasterVp");
	pass->getVertexProgramParameters()->setNamedAutoConstant(
		"worldViewProj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
	pass->setFragmentProgram("CustomShadowCasterFp");
	*/

	//Set debugging option
#ifdef _DEBUG
	mPhysicsManager->setDebugMode(mSceneManager);
#endif

	//Ogre::CompositorManager::getSingleton().setCompositorEnabled(mWindow->getViewport(0), compName, true);

	//Set skybox three types = WoodSkyBox,TrippySkyBox,SpaceSkyBox
	//mSceneManager->setSkyBox(true, "WoodSkyBox", 300, false);

	// Set shadow option
	mSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	mSceneManager->setShadowColour(Ogre::ColourValue(0, 0, 0));
	mSceneManager->setAmbientLight(Ogre::ColourValue(0.3f, 0.3f, 0.3f));
	//@PA: I added a new Skybox texture.
	setSkybox("Sky");
	//APPLICATION->setSkybox("WoodSkyBox");


#endif

	return 0;
}



int ssurge::Application::update(double dt)
{
	mPhysicsManager->update(dt);
	mGameObjectManager->update(dt);

	return 1;
}

void ssurge::Application::setSkybox(std::string Name)
{
	//Set skybox three types = WoodSkyBox,TrippySkyBox,SpaceSkyBox,Sky
	mSceneManager->setSkyBox(true, Name, 300, false);
}

void ssurge::Application::toggleSkybox(bool toggle,std::string Name)
{
	//Set skybox three types = WoodSkyBox,TrippySkyBox,SpaceSkyBox,Sky
	mSceneManager->setSkyBox(toggle, Name, 300, false);
}
