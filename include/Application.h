#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <stdafx.h>
#include <LogManager.h>
#include <Singleton.h>
#include <GameObjectManager.h>
#include <InputManager.h>
#include <ScriptManager.h>
#include <LScriptManager.h>
#include <SoundManager.h>
#include <PhysicsManager.h>
#include <CameraComponent.h>
#include <DebugDrawer.h>
#include <FullscreenShaderManager.h>
#include <GUIManager.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <NetworkManager.h>

/// Convenience macro to make accessing the singleton a bit less cumbersome
#define APPLICATION ssurge::Application::getSingletonPtr()

namespace ssurge
{
	/// The various states the application can be in.
	enum class ApplicationState { created, initialized, running, readyToQuit, dead };


	/// The Application class is basically the hub of the entire program.
	/// It should contain all of (ssurge's) managers and all shutdown
	/// and startup code.  Everything else should (eventually) be
	/// delegated to the managers.
	class Application : public Singleton<Application>
	{
	/***** ATTRIBUTES *****/
	protected:
		/// The Ogre Root objects
		Ogre::Root * mRoot;

		/// The Ogre-controlled window we're rendering to
		Ogre::RenderWindow * mWindow;

		/// The InputManager object
		InputManager * mInputManager;

		/// The state of the application
		ApplicationState mState;

		/// The LogManager
		LogManager * mLogManager;

		/// The Ogre Scene Manager
		Ogre::SceneManager * mSceneManager;

		/// The Game Object Manager
		GameObjectManager * mGameObjectManager;

		/// The ScriptManager
		ScriptManager * mScriptManager;

		/// The Lua ScriptManager
		LuaScriptManager * mLuaScriptManager;

		/// The ScriptManager
		SoundManager * mSoundManager;

		/// The PhysicsManager
		PhysicsManager * mPhysicsManager;

		/// The Fullscreen (shader) effect manager
		FullscreenShaderManager * mFullscreenEffectManager;

		/// The GUIManager
		GUIManager * mGUIManager;

		/// The NetworkManager
		NetworkManager * mNetworkManager;

		///The Camera Component
		CameraComponent * mCameraComponent;

		/// The Ogre Overlay system (currently used by the FullscreenShaderManager class)
		Ogre::OverlaySystem * mOverlaySystem;

		/// An Ogre Timer object (used to calculate time between frames)
		Ogre::Timer mTimer;

		bool mTerrainsImported;
		Ogre::TerrainGroup* mTerrainGroup;
		Ogre::TerrainGlobalOptions* mTerrainGlobals;

	/***** CONSTRUCTORS / DESTRUCTORS *****/
	public:
		/// The Basic constructor.  Doesn't do much -- call initialize to fully initialize the application
		Application();

		/// Frees any memory we allocated and calls the shutdown method.
		virtual ~Application();

		/// Initializes the application.  Call this before calling run.  Returns 0 on success.  If we
		/// think we might sub-class the Application class, it would probably be smart to break this method up.
		virtual int initialize(std::string window_title, bool local, bool server, bool strict);

		/// The shutdown method.   Returns 0 on success.
		virtual int shutdown();


	/***** METHODS *****/
	public:
		/// Starts the internal game loop.  Returns 0 if we had a clean end to the main loop.
		int run();

		/// Gets the scene manager
		Ogre::SceneManager * getSceneManager() { return mSceneManager; }

		/// Sets the background color
		void setBackgroundColor(Ogre::ColourValue c) { if (mWindow->getNumViewports() > 0)  mWindow->getViewport(0)->setBackgroundColour(c); }

		/// Sets the active camera (this could also create the viewport if there is none)
		void setActiveCamera(Ogre::Camera * c);

		/// Gets the aspect ratio of the render window
		float getAspectRatio() { return (float)mWindow->getWidth() / mWindow->getHeight(); }

		///@PA: This is a temporary way to get a Camera Component set up.
		void setCameraComponent(CameraComponent* c) { setActiveCamera(c->getCamera()); mCameraComponent = c; }

		///@JT: Gets the camera component
		CameraComponent * getCameraComponent() { return mCameraComponent; }

		//set the skybox 
		void setSkybox(std::string name);

		//disable skybox
		void toggleSkybox(bool toggle, std::string name);

	protected:
		/// Sets up the scene
		int createScene();

		/// Does any internal updates (called once per frame (before rendering)).  If this method
		/// returns 0, the game shuts down.
		int update(double dt);
	};

}

#endif