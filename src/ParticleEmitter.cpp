
#include <stdafx.h>
#include <ParticleEmitter.h>
#include <Application.h>
#include <string>


ssurge::ParticleEmitter::ParticleEmitter(GameObject* owner, std::string partName): Component(owner)
{
	particle_effect = APPLICATION->getSceneManager()->createParticleSystem("Test", partName);
	mOwner->addComponent(this);
	//Ogre::SceneNode* particleNode = APPLICATION->getSceneManager()->getRootSceneNode()->createChildSceneNode("Particle");
	Ogre::SceneNode* particleNode = mOwner->getSceneNode();

	particleNode->attachObject(particle_effect);
}

ssurge::ParticleEmitter::~ParticleEmitter()
{


}

ssurge::ComponentType ssurge::ParticleEmitter::getType()
{
	return ComponentType::PARTICLE;
}

void ssurge::ParticleEmitter::update(double dt)
{
	
}
