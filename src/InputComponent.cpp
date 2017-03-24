#include <stdafx.h>
#include <InputComponent.h>
#include <SimplePlayer.h>
#include <GameObjectManager.h>
#include <InputManager.h>
#include <utility.h>
#include <MeshComponent.h>

//@KJ: We need to attach it to the parent -- see MeshComponent's constructor.

ssurge::InputComponent::InputComponent(ssurge::GameObject * owner) : Component(owner)
{

	mOwner->addComponent(this);
}

ssurge::InputComponent::~InputComponent()
{

}
//updating the postion of a mesh
void ssurge::InputComponent::update(double dt)
{
	//@KJ: A lot of this will go away (especially the orb stuff)
	Ogre::Vector3 new_dir = Ogre::Vector3(INPUT_MANAGER->getAxis(0, true), INPUT_MANAGER->getAxis(0, false), 0);
	new_dir.normalise();
	mOwner->translateWorld(new_dir * static_cast<float>(mSpeed * dt));
	bool moving = new_dir.dotProduct(new_dir) > 0;
	float radBetween = fabs(new_dir.angleBetween(mCurrentDirection).valueRadians());
	if (moving && radBetween > 0.001f)
	{
		Ogre::Vector3 axis = new_dir.crossProduct(mCurrentDirection);
		mOwner->rotateWorld(static_cast<float>(90.0 * dt), axis);
		mCurrentDirection = mOwner->getSceneNode()->getOrientation().yAxis();
	}

	for (unsigned int i = 0; i < mOrbs.size(); i++)
	{
		mOrbs[i]->rotateWorld(static_cast<float>(mOrbSpeeds[i] * dt), Ogre::Vector3(0, 0, 1));
	}

}
