#include <stdafx.h>
#include <SimplePlayer.h>
#include <GameObjectManager.h>
#include <InputManager.h>
#include <utility.h>
#include <MeshComponent.h>
#include <GUIManager.h>


ssurge::SimplePlayer::SimplePlayer(std::string name, unsigned int tag) : GameObject(name, tag), mNextOrbID(0)
{
	// Load and position the sword.
	mSword = GAME_OBJECT_MANAGER->createGameObject(name + "_sword", tag);
	// New method for creating mesh components!
	MeshComponent * mesh_comp = new MeshComponent(mSword);
	mesh_comp->loadMesh("sword.mesh");

	mSword->setScale(0.4f, 0.4f, 0.4f);
	mSword->setPosition(0.5f, -2, -0.5f);
	mSword->rotateLocal(180.0f, Ogre::Vector3(1, 0, 0));
	mCurrentDirection = Ogre::Vector3(0, -1, 0);			// This is the direction the monkey was modelled.

	// Set the default speed
	mSpeed = 2.0f;

	// Add this object as an input listeners
	INPUT_MANAGER->addListener(this);

	// This looks pointless, but it moves the sword out of the scene graph
	addChild(mSword);     // Makes it our child
	removeChild(mSword);		  // Makes it no one's child.
}

ssurge::SimplePlayer::~SimplePlayer()
{

}


void ssurge::SimplePlayer::handleAction(std::string action, bool isStarting)
{
	if (action == "attack")
	{
		if (isStarting)
		{
			ssurge::MeshComponent * mc = static_cast<MeshComponent*>(GAME_OBJECT_MANAGER->findGameObject("Ninja")->getComponent(ssurge::ComponentType::MESH));
			mc->queueAnim("Attack2", false, "upper");
		}
	}
	else if (action == "jump" && isStarting)
	{
		//temporary test code for log
		GUI_MANAGER->logText("jumping");
		/*
		// Create two scene nodes -- one for rotation and a child node which is for the "arm" of rotation (the former
		// is the one added to mOrbs.
		char temps[256];
		sprintf_s(temps, "OrbRotater%d", mNextOrbID);
		GameObject * rotater = GAME_OBJECT_MANAGER->createGameObject(temps, 11);
		rotater->rotateWorld(rand_float(0.0f, 360.0f), Ogre::Vector3(0, 1, 0));
		GameObject * arm = GAME_OBJECT_MANAGER->createGameObject(std::string(temps) + "_arm", 11);
		GameObject * scaler = GAME_OBJECT_MANAGER->createGameObject(std::string(temps) + "_scaler", 11);
		arm->setPosition(rand_float(2.5f, 6.5f), 0, 0.3f);
		float sfactor = rand_float(0.3f, 0.6f);
		scaler->setScale(sfactor, sfactor, sfactor);

		MeshComponent * ico = new MeshComponent(scaler);
		ico->loadMesh("Icosphere.mesh");

		addChild(rotater);
		rotater->addChild(arm);
		arm->addChild(scaler);
		mOrbs.push_back(rotater);
		if (rand_float(0.0f, 1.0f) < 0.5f)
			mOrbSpeeds.push_back(rand_float(-180.0f, -15.0f));
		else
			mOrbSpeeds.push_back(rand_float(15.0f, 180.0f));
		mNextOrbID++;*/

		ssurge::MeshComponent * mc = static_cast<MeshComponent*>(GAME_OBJECT_MANAGER->findGameObject("Ninja")->getComponent(ssurge::ComponentType::MESH));
		mc->playAnim("Jump", false);
	}
	else if (action == "sprint")
	{
		if (isStarting)
		{
			ssurge::MeshComponent * mc = static_cast<MeshComponent*>(GAME_OBJECT_MANAGER->findGameObject("Ninja")->getComponent(ssurge::ComponentType::MESH));
			mc->playAnim("Walk", true, "lower");
			mc->setMaterial(0, "Examples/Ninjatest");
			mc->setMaterial(1, "Examples/Ninjatest");
		}
		else
		{
			ssurge::MeshComponent * mc = static_cast<MeshComponent*>(GAME_OBJECT_MANAGER->findGameObject("Ninja")->getComponent(ssurge::ComponentType::MESH));
			mc->playDefaultAnim();
			mc->setMaterial(0, "Examples/Ninja");
			mc->setMaterial(1, "Examples/Ninja");
		}
	}
}

/*void ssurge::SimplePlayer::update(double dt)
{
	/*
	Ogre::Vector3 new_dir = Ogre::Vector3(INPUT_MANAGER->getAxis(0, true), INPUT_MANAGER->getAxis(0, false), 0);
	new_dir.normalise();
	translateWorld(new_dir * static_cast<float>(mSpeed * dt));
	bool moving = new_dir.dotProduct(new_dir) > 0;
	*/
	// "Easy" direction snapping
	/*if (moving && fabs(new_dir.angleBetween(mCurrentDirection).valueRadians()) > 0.001f)
	{
		rotateWorld(mCurrentDirection.getRotationTo(new_dir, Ogre::Vector3(0, 0, 1)));
		mCurrentDirection = new_dir;
	}*/
	/*
	// Smoother direction transition
	float radBetween = fabs(new_dir.angleBetween(mCurrentDirection).valueRadians());
	if (moving && radBetween > 0.001f)
	{
		Ogre::Vector3 axis = new_dir.crossProduct(mCurrentDirection);
		rotateWorld(static_cast<float>(90.0 * dt), axis);
		mCurrentDirection = mSceneNode->getOrientation().yAxis();
	}
	
	for (unsigned int i = 0; i < mOrbs.size(); i++)
	{
		mOrbs[i]->rotateWorld(static_cast<float>(mOrbSpeeds[i] * dt), Ogre::Vector3(0, 0, 1));
	}
}*/