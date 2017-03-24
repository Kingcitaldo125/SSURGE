#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <stdafx.h>
#include <Tuple.h>

namespace ssurge
{
	// A forward-reference to the GameObject class (put here to avoid a circular
	// dependency)
	class GameObject;


	/// This is an enumeration of component types.  It is the "master list" of these types
	/// Make sure to update this when creating new component types.  Also note the static initializeComponentNames
	/// method in the Component class.  Ensure this is called at least once.  Afterwards, the
	/// static getComponentName method can be called.
	enum class ComponentType 
	{
		UNKNOWN,
		MESH,
		CAMERA,
		SOUND,
		REVERB_ZONE,
		COLLIDER,
		RIGIDBODY,
		PARTICLE
	};

	/// The component class is a base class for all future component types
	class Component
	{
	/***** STATIC ATTRIBUTES *****/
	protected:
		/// A master list of all component type -> name string conversions
		static std::map<ssurge::ComponentType, std::string> mTypeNames;

		/// A master list of all operations that are performable by given component types
		static std::map<std::string, ssurge::ComponentType> mOperations;

	/***** ATTRIBUTES *****/
	protected:
		/// The game object that owns this component
		GameObject * mOwner;

	/***** CONSTRUCTORS / DESTRUCTORS *****/
	public:
		/// The default constructor
		Component(GameObject * owner);

		/// The destructor
		virtual ~Component() {  }

	/***** STATIC METHODS *****/
	public:
		/// Converts a ComponentType to a string.  Make sure initializeComponentNames has been
		/// called or this will always return an empty string
		static std::string getComponentTypeNameFromType(ComponentType t);

		/// Converts a string to a ComponentType.  Make sure initializeComponentNames has been
		/// called or this will always return UNKNOWN.
		static ComponentType getComponentTypeFromString(std::string s);

		/// Returns the component type which is capable of performing the operation given by name
		static ComponentType getComponentTypeForOperation(std::string o);

		/// Registers a string as an identifier for a new operation performable by the given component type
		/// Returns true when the registration was successful, returns false otherwise
		static bool registerOperation(std::string o, ComponentType t);

		/// Initializes the component type -> type name string map and the operation name string -> component type map.
		/// Make sure to use this (and not hard-code string values...that's a bug waiting to happen).
		static void initializeComponentMaps();

		/// Destroy the maps created by initializeComponentsMaps()
		static void destroyComponentMaps();

	/***** METHODS *****/
	public:
		/// Returns a component type enum for this component.  Derived classes should have knowledge of this.
		virtual ComponentType getType() = 0;

		/// Do any updating this component needs. 
		virtual void update(double dt) = 0;

		/// Called by the owner GameObject when it moves.
		virtual void ownerMoved(const Ogre::Node * n) {}

		/// Forwarded by the owner GameObject from the Network Manager when it is instructed by an incoming packet to handle an operation
		virtual Tuple * handleOperation(std::string o, Tuple * t) { return NULL; }

	/***** FRIEND CLASSES *****/
		friend class GameObject;
	};
}

#endif
