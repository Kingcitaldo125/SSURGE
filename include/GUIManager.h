#ifndef _GUI_MANAGER_H_
#define _GUI_MANAGER_H_

#include <stdafx.h>
#include <Singleton.h>
#include <vector>

// Convenience macro to make accessing the GameObjectManager singleton a bit less cumbersome
#define GUI_MANAGER ssurge::GUIManager::getSingletonPtr()

namespace ssurge
{

	/// This class manages all game objects in the game.  It also contains
	/// functionality to read in an ogre .scene (xml) file and populate a scene.
	class GUIManager : public Singleton < GUIManager >
	{
	private:
		/***** ATTRIBUTES *****/
		
		/// pointer to overlay manager
		Ogre::OverlayManager * omgr;

		/// pointer to panel container
		Ogre::OverlayContainer * pcont;

		/// pointer to log container 
		Ogre::OverlayContainer * lcont;

		/// internal count for each unique text overlay
		int textCount;

		/// internal count for each unique image overlay
		int imageCount;

		/// vector storing the log texts
		std::vector<std::string> textLog;

		///
		std::string logString;

	public:

		/***** CONSTRUCTORS / DESTRUCTORS *****/
		GUIManager();
		virtual ~GUIManager();

		/***** METHODS *****/
	public:
		/// creates text on ogre overlay at given position. last argument is not necesary, only logtext() changes the arg
		int virtual createTextOverlay(Ogre::String text, float x, float y, Ogre::String panel = "GUIPanel");

		/// creates image on ogre overlay at given position
		int virtual createImageOverlay(Ogre::String img, float x, float y);

		/// destroys given overlay by overlay type and counter (Image1, Image3, Text17, etc...)
		void virtual removeOverlay(Ogre::String name);

		/// move given overlay by name
		void virtual moveOverlay(Ogre::String name, float x, float y);

		/// logs text to screen in rolling chat window
		void logText(Ogre::String text);
	};

}

#endif