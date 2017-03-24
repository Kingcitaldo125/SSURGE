#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include <stdafx.h>
#include <Component.h>
#include <ScriptTopLevel.h>
#include <ScriptGameObject.h>
#include <Tuple.h>

namespace ssurge
{
	// A forward reference to the GameObjectManager class
	class GameObjectManager;

	/// A GameObject is a "thing" in the game.  By adding components to it, we add functionality.
	class GameObject : public Ogre::Node::Listener
	{
	/***** ATTRIBUTES *****/
	protected:
		/// The scene node containing the transformation data (and all attached Ogre objects)
		Ogre::SceneNode * mSceneNode;

		/// The "tag" of this game object.  This is a user-defined value (could be used to mark
		/// players, enemies, level0 objects, etc.)
		unsigned int mTag;

		/// The name of the game object.  This is the key in GameObjectManager's master list of all objects
		std::string mName;

		/// A dictionary of components.  Note: for now, we only allow once component of each type.  If this
		/// design goal changes, we could make this a dictionary of linked-lists (or equivalent)
		std::map<ComponentType, Component*> mComponents;

		/// The script object that is the "twin" of this GameObject in the Python universe.  If this is NULL,
		/// it means this is a purely-C++ game object
		PyObject * mScriptObject;

		/// keyword args (equivalent to Python *kwargs)
		PyObject * mPythonKwargs = PyDict_New();

		/// python args (args that do not change)
		PyObject * mPythonArgs;

		/// This value determines whether a failure was signalled by a component owned by this game object during
		/// execution of handleOperation
		bool mComponentOperationFailure;

	/***** CONSTRUCTORS / DESTRUCTORS *****/
	protected:
		/// Default constructor
		GameObject(std::string name, unsigned int tag);

		/// This constructor places the new game object as a child of the given parent
		GameObject(std::string name, unsigned int tag, GameObject * parent);

	public:
		/// Destructor
		virtual ~GameObject();
	

	/***** METHODS *****/
	public:
		/// Adds a component.  This will (currently) replace an existing component of that type.
		/// IMPORTANT: by calling this method, the caller is giving up memory ownership of the
		/// (hopefully) dynamically created component they just passed us.  The GameObject will 
		/// dispose of that component in its destructor.
		virtual void addComponent(Component * c);

		/// This method remove (and returns the component of the given type).  NULL will be
		/// returned if there is no element of that type.
		Component * removeComponent(ComponentType ctype);

		/// This method destroys the component of the given type (if it exists)
		virtual void destroyComponent(ComponentType t);

		/// [Temporary] Adds a camera
		virtual int addCamera(Ogre::Camera * c, bool make_active);

		/// [Temporary] Adds a point light
		virtual int addLight(Ogre::Light * L);

		/// Get the name of this game object
		virtual std::string getName() const { return mName; }

		/// Gets the tag of this game object
		virtual unsigned int getTag() const { return mTag; }

		/// Does a down-cast (i.e. dynamic_cast) to convert a Component* to a particular type...if this
		/// game object has a component of that type.  If not, NULL is returned.  This method will likely
		/// die horribly if the user doesn't call it with a Component-derived template class
		//template <class T> T * getComponent(ComponentType t)
		Component * getComponent(ComponentType t)
		{
			//static_assert(std::is_base_of(Component, T), "T must be derived from ssurge::Component");
			std::map<ComponentType, Component*>::iterator iter;
			iter = mComponents.find(t);
			if (iter == mComponents.end())
				return NULL;
			return iter->second;
		}

		/// Removes a child GameObject.  This does *not* destroy the scene node or its contents.
		virtual void removeChild(GameObject * g);

		/// Adds a child game object
		virtual void addChild(GameObject * go);

		/// Forwarded by the Network Manager when it is instructed by an incoming packet to handle an operation
		/// Should forward the operation onto a component which can perform it
		/// Returns true when successful, returns false otherwise
		virtual Tuple * handleOperation(PyObject * args);

		/// Forwarded by the Network Manager when it is instructed by an incoming packet to handle an operation
		/// Should forward the operation onto a component which can perform it
		/// Returns true when successful, returns false otherwise
		virtual Tuple * handleOperation(std::string o, Tuple * t);

		/// [Temporary, until we have scripting]
		virtual void handleAction(std::string action, bool isStarting) {}

		/// [Temporary, until we have scripting]
		virtual void update(double dt);

		/// set python args (these are args that do not change)
		virtual void setPythonArgs(PyObject * tuple);

		/// add a kwarg to the constructor args
		virtual void setPythonKwarg(PyObject * key, PyObject * val);

		/// called by a component owned by this game object to signal a failure during handleOperation
		virtual void signalOperationFailure() { mComponentOperationFailure = true; };


	/***** CALLBACKS (called by someone else) *****/
		/// Called by Ogre every time 
		virtual void nodeUpdated(const Ogre::Node*);

	/***** GETTERS / SETTERS *****/
	protected:
		/// Sets the script (Python/C) "twin" of this GameObject.  Doing this makes this object "script-aware" -- it becomes part of the 
		/// python interpreter.  I've made this method protected (and made the ScriptManager and createGameObject [ScriptTopLevel.cpp] friend classes / functions)
		/// to safeguard who calls it
		

		/// Removes the script object (this decref's the object in the python universe, which should usually cause a 
		/// garbage cleanup (unless someone else is retaining a reference)
		virtual void removeScriptObject();

	public:
		//TEMPORARILY MOVED 
		virtual void setScriptObject(PyObject * twin);

		/// Gets the scene node.  Do we want to allow this???
		Ogre::SceneNode * getSceneNode() { return mSceneNode; }

		/// Sets the position of this GameObject (this is always relative to the parent node)
		virtual void setPosition(const float x, const float y, const float z) { mSceneNode->setPosition(x, y, z); }

		/// Sets the position of this GameObject (this is always relative to the parent node)
		virtual void setPosition(const Ogre::Vector3 & v) { mSceneNode->setPosition(v); }

		/// Sets the orientation of this GameObject (this is always relative to the parent node)
		virtual void setOrientationAxisAngle(const float degrees, const float axisX, const float axisY, const float axisZ) { mSceneNode->setOrientation(Ogre::Quaternion(Ogre::Degree(degrees), Ogre::Vector3(axisX, axisY, axisZ))); }

		/// Sets the orientation of this GameObject (this is always relative to the parent node)
		virtual void setOrientationQuaternion(const float x, const float y, const float z, const float w) { mSceneNode->setOrientation(Ogre::Quaternion(w, x, y, z)); }

		/// Sets the orientation of this GameObject (this is always relative to the parent node)
		virtual void setOrientationQuaternion(const Ogre::Quaternion & q) { mSceneNode->setOrientation(q); }

		/// Sets the scale of this GameObject
		virtual void setScale(const float sx, const float sy, const float sz) { mSceneNode->setScale(sx, sy, sz); }

		/// Sets the scale of this GameObject
		virtual void setScale(const Ogre::Vector3 & s) { mSceneNode->setScale(s); }

		/// Rotates the object relative to their local coordinate space
		virtual void rotateLocal(float degrees, Ogre::Vector3 axis) { mSceneNode->rotate(Ogre::Quaternion(Ogre::Degree(degrees), axis), Ogre::Node::TS_LOCAL); }

		/// Rotates the object relative to the world coordinate space
		virtual void rotateWorld(float degrees, Ogre::Vector3 axis) { mSceneNode->rotate(Ogre::Quaternion(Ogre::Degree(degrees), axis), Ogre::Node::TS_WORLD); }

		/// Rotates the object relative to the world coordinate space
		virtual void rotateWorld(Ogre::Quaternion q) { mSceneNode->rotate(q, Ogre::Node::TS_WORLD); }

		/// Translate the object relative to its local coordinate space
		virtual void translateLocal(Ogre::Vector3 v) { mSceneNode->translate(v, Ogre::Node::TS_LOCAL); }

		/// Translate the object relative to the world coordinate space
		virtual void translateWorld(Ogre::Vector3 v) { mSceneNode->translate(v, Ogre::Node::TS_WORLD); }

		/// Removes a child GameObject (by index, the order it was added, starting with 0).  This does *not* destroy the scene node or its contents.
		virtual void removeChild(unsigned int index);

	/***** FRIENDS *****/
		friend class GameObjectManager;
		friend class SoundManager;
	};

}

#endif