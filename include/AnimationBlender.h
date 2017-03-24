#include <stdafx.h>


namespace ssurge
{
	class AnimationBlender
	{
	public:
		enum BlendingTransition
		{
			BlendSwitch,         // stop source and start dest
			BlendWhileAnimating,   // cross fade, blend source animation out while blending destination animation in
			BlendThenAnimate      // blend source to first frame of dest, when done, start dest anim
		};

	private:
		Ogre::Entity *mEntity;
		Ogre::AnimationState *mSource;
		Ogre::AnimationState *mTarget;

		BlendingTransition mTransition;

		bool loop;

		~AnimationBlender() {}

	public:
		Ogre::Real mTimeleft, mDuration;

		bool complete;

		void blend(const std::string &animation, BlendingTransition transition, Ogre::Real duration, bool l = true);
		void addTime(Ogre::Real);
		Ogre::Real getProgress() { return mTimeleft / mDuration; }
		Ogre::AnimationState *getSource() { return mSource; }
		Ogre::AnimationState *getTarget() { return mTarget; }
		AnimationBlender(Ogre::Entity *);
		void init(const std::string &animation, bool l = true);
	};
}