#pragma once

#include <stdafx.h>
#include <Singleton.h>
#include <btBulletDynamicsCommon.h>
#include <RigidBodyComponent.h>
#include <ColliderComponent.h>
#include <DebugDrawer.h>

/// Convenience macro to make accessing the PhysicsManager singleton a bit less cumbersome
#define PHYSICS_MANAGER ssurge::PhysicsManager::getSingletonPtr()

namespace ssurge
{
	class PhysicsManager : public Singleton<PhysicsManager>
	{
	/***** ATTRIBUTES *****/
	private:
		/// Excellent for eliminating object pairs that should not collide
		btBroadphaseInterface * mBroadphase;

		/// Allows us to fine tune the algorithms used for the full (not broadphase) collision detection
		btDefaultCollisionConfiguration * mCollisionConfiguration;

		/// Takes the collision configuration and applies it
		btCollisionDispatcher * mDispatcher;

		/// Causes the objects to interact properly (gravity, forces, hinge constraints)
		btSequentialImpulseConstraintSolver * mSolver;

		/// The dynamics world where physics will take place
		btDiscreteDynamicsWorld * mDynamicsWorld;

		/// The list of all colliders, keys are the same as the game object names they are attached to
		std::map<std::string, btCollisionShape*> mColliders;

		/// The list of all rigid-bodies, keys are the same as the game object names they are attached to
		std::map<std::string, btRigidBody*> mRigidBodies;


	/***** CONSTRUCTORS / DESTRUCTORS *****/
	public:
		/// Basic constructor sets up the dynamics world
		PhysicsManager();

		/// Frees any memory we allocated.
		~PhysicsManager();


	/***** METHODS *****/
	public:
		/// Updates the dynamics world
		void update(double dt);

		/// Adds a collider to the dictionary mColliders
		void addCollider(std::string name, btCollisionShape * cs);

		/// Removes a single specified collider
		void removeCollider(std::string name);

		/// Adds a rigid-body to the dictionary mRigidBodies
		void addRigidBody(std::string name, btRigidBody * rb);

		/// Removes a single specified rigid-body
		void removeRigidBody(std::string name);

		/// Destroys all colliders
		void destroyAllColliders();

		/// Destroys all rigid-bodies
		void destroyAllRigidBodies();

		/// Sets up the debug mode to be used
		void setDebugMode(Ogre::SceneManager * sceneManager);
	};
}
