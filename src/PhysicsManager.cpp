#include <stdafx.h>
#include <PhysicsManager.h>
#include <DebugDrawer.h>

// Template specialization to initialize and declare the singleton pointer for PhysicsManager
template<>
ssurge::PhysicsManager * ssurge::Singleton<ssurge::PhysicsManager>::msSingleton = nullptr;

ssurge::PhysicsManager::PhysicsManager() : mBroadphase(NULL), mCollisionConfiguration(NULL), mDispatcher(NULL), 
mSolver(NULL), mDynamicsWorld(NULL)
{
	// Build the broadphase
	mBroadphase = new btDbvtBroadphase();

	// Set up the collision configuration and dispatcher
	mCollisionConfiguration = new btDefaultCollisionConfiguration();
	mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);

	// The actual physics solver
	mSolver = new btSequentialImpulseConstraintSolver;

	// The dynamics world
	mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);
	mDynamicsWorld->setGravity(btVector3(0, 0, -10));

}


ssurge::PhysicsManager::~PhysicsManager()
{
	destroyAllRigidBodies();
	destroyAllColliders();

	delete mDynamicsWorld;
	delete mSolver;
	delete mDispatcher;
	delete mCollisionConfiguration;
	delete mBroadphase;
}



void ssurge::PhysicsManager::update(double dt)
{

#ifdef _DEBUG
	mDynamicsWorld->debugDrawWorld();
#endif
	mDynamicsWorld->stepSimulation(1 / 60.f, 10, dt / 4.0);
}


void ssurge::PhysicsManager::addCollider(std::string name, btCollisionShape * cs)
{
	mColliders[name] = cs;
}


void ssurge::PhysicsManager::removeCollider(std::string name)
{
	mColliders.erase(name);
}


void ssurge::PhysicsManager::addRigidBody(std::string name, btRigidBody * rb)
{
	mDynamicsWorld->addRigidBody(rb);
	mRigidBodies[name] = rb;
}


void ssurge::PhysicsManager::removeRigidBody(std::string name)
{
	mDynamicsWorld->removeRigidBody(mRigidBodies[name]);
	mRigidBodies.erase(name);
}


void ssurge::PhysicsManager::destroyAllColliders()
{
	std::map<std::string, btCollisionShape*>::iterator iter = mColliders.begin();
	while (iter != mColliders.end())
	{
		delete iter->second;
		iter->second = NULL;
	}
	mColliders.clear();
}


void ssurge::PhysicsManager::destroyAllRigidBodies()
{
	std::map<std::string, btRigidBody*>::iterator iter = mRigidBodies.begin();
	while (iter != mRigidBodies.end())
	{
		mDynamicsWorld->removeRigidBody(iter->second);
		delete iter->second->getMotionState();
		delete iter->second;
		iter->second = NULL;
	}
	mRigidBodies.clear();
}


void ssurge::PhysicsManager::setDebugMode(Ogre::SceneManager * sceneManager)
{
	OgreDebugDrawer *debugDrawer = new OgreDebugDrawer(sceneManager);
	debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	mDynamicsWorld->setDebugDrawer(debugDrawer);
}
