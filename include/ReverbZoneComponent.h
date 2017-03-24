#ifndef _REVERB_ZONE_COMPONENT_H_
#define _REVERB_ZONE_COMPONENT_H_

#include <stdafx.h>
#include <Component.h>
#include <GameObject.h>
#include <SoundManager.h>

namespace ssurge
{
	class ReverbZoneComponent : public Component
	{
	/***** ATTRIBUTES *****/
	protected:

		float mMix;

		float mDecay;

		float mOut;

		float mIn;

	/***** CONSTRUCTORS / DESTRUCTORS *****/
	public:

		ReverbZoneComponent(GameObject * owner, float inDist, float outDist, float mix, float decayTime) : Component(owner),
			mOut(outDist), mIn(inDist), mMix(mix), mDecay(decayTime) { SOUND_MANAGER->addReverbZone(this); };

		~ReverbZoneComponent() { SOUND_MANAGER->removeReverbZone(this); };

	/***** METHODS *****/
		float getInfluence(Ogre::Vector3 pos) { return std::min(1.0f, std::max(0.0f,
				(mOut - mOwner->getSceneNode()->_getDerivedPosition().distance(pos)) / (mOut - mIn))); };

		ComponentType getType() override { return ComponentType::REVERB_ZONE; };

		virtual void update(double dt) {};

	/***** GETTERS / SETTERS *****/
	public:
		float getMix() { return mMix; };

		float getDecayTime() { return mDecay; };
	};
}

#endif