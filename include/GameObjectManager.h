#ifndef _GAME_OBJECT_MANAGER_H_
#define _GAME_OBJECT_MANAGER_H_

#include <stdafx.h>
#include <Singleton.h>
#include <GameObject.h>

// Convenience macro to make accessing the GameObjectManager singleton a bit less cumbersome
#define GAME_OBJECT_MANAGER ssurge::GameObjectManager::getSingletonPtr()

namespace ssurge
{

	/// This class manages all game objects in the game.  It also contains
	/// functionality to read in an ogre .scene (xml) file and populate a scene.
	class GameObjectManager : public Singleton < GameObjectManager >
	{
	/***** ATTRIBUTES *****/
	protected:
		/// The master list of all game objects
		std::map<std::string, GameObject*> mObjects;
	
	/***** NESTED CLASSES *****/
	protected:
		/// This is a simple class which will have its for_each method called on
		/// each node in the xml file (this process is initiated in GameObjectManager::loadXML.
		/// Note: because this is protected, any code outside of the GameObjectManager
		/// class won't even be aware this exists (which is what I want).
		class DataWalker : public pugi::xml_tree_walker
		{
		protected:
			/// The GameObjectManager singleton -- for convenience
			GameObjectManager * mManager;

			/// The camera tag to look for -- if a game object with this tag is found, the camera
			/// will be made active.
			unsigned int mCameraTag;
		public:
			/// Default constructor
			DataWalker(GameObjectManager * gom, unsigned int ct) : mManager(gom), mCameraTag(ct) {} 
			
			/// This method is called once for each node.
			virtual bool for_each(pugi::xml_node & node);

		protected:
			/// Finds the parent game object (if any) of this node (NULL is returned if this game object is to be a 
			/// child of the root)
			GameObject * findParent(pugi::xml_node & node);
			
			/// Find the name of this game object node (if there is none, an ssurge::Exception is thrown)
			std::string findName(pugi::xml_node & node);

			/// Find the tag of this game object node (if there is none, the default returned is 0)
			int findTag(pugi::xml_node & node);

			/// Process the transformation data within this node -- set the already-built game-object to match
			void processTransforms(pugi::xml_node & node, GameObject * gobj);

			/// Create a mesh object and attach it to the game object (if the node has a "entity" child node)
			void processMesh(pugi::xml_node & node, GameObject * gobj);

			/// Create a light object and attach it to the game object (if the node has a "light" child node)
			void processLight(pugi::xml_node & node, GameObject * gobj);

			/// Create a camera object and attach it to the game object (if the node has a "camera" child node).
			/// Additionally, if this node has a tag of 100, it is set as the "active" camera used by the
			/// Application when rendering.
			void processCamera(pugi::xml_node & node, GameObject * gobj);

			/// Create a collider and rigid body objects based off the given shape ie:(plane, box, sphere, capsule, cone, cylinder).
			/// Mass is an optional attribute, but is set to 0 if not given so it is suggested to give set the mass tag.
			/// Additionally, has two optional tags: friction and restitution (bounciness).
			/// Friction should be set to a float between 0 and 1, where 1 will usually stop an object in it's tracks.
			/// Restitution should be set to a float between 0 and 1.99, where 2 and greater will bounce to exponential heights.
			void processCollider(pugi::xml_node & noe, GameObject * gobj);

			/// Get an Ogre Vector3 (defaults to (0,0,0) using the attributes "x", "y", "z")
			Ogre::Vector3 getVector3(pugi::xml_node & node);

			/// Get an Ogre ColourValue (defaults to (0,0,0) using the attributes "r", "g", "b")
			Ogre::ColourValue getColour(pugi::xml_node & node);

			/// Get an Ogre ColourValue (defaults to (1, 0,0,0) using the attributes "qw", "qx", "qy", "qz")
			Ogre::Quaternion getQuaternion(pugi::xml_node & node);
		};

	/***** CONSTRUCTORS / DESTRUCTORS *****/
	public:

	/***** METHODS *****/
	public:
		/// Loads a batch of game objects from an xml file.  Returns 0 on success
		/// If a camera is found with the given tag, it will be made the active camera
		/// (and will render to application's viewport #0)
		int loadXML(std::string fname, unsigned int cameraTag = 99999999);

		/// Destroys all game objects
		void destroyAll();

		/// Finds the GameObject with the given name (if it's not found, NULL is returned)
		GameObject * findGameObject(std::string name);

		/// Calls the update method of all GameObjects
		void update(double dt);

		/// Creates a new GameObject and adds it to the master list
		GameObject * createGameObject(std::string name, unsigned int tag) { GameObject * go = new GameObject(name, tag);  mObjects[name] = go; return go; }
	};

}

#endif