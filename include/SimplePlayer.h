#ifndef _SIMPLE_PLAYER_H_
#define _SIMPLE_PLAYER_H_

#include <stdafx.h>
#include <GameObject.h>

namespace ssurge
{
	/// This is a *VERY* temporary class.  Once scripting is more functional,
	/// we'll have no need for this class.
	class SimplePlayer : public GameObject
	{
	protected:
		GameObject * mSword;
		std::vector<float> mOrbSpeeds;
		std::vector<GameObject*> mOrbs;
		unsigned int mNextOrbID;
		double mSpeed;
		Ogre::Vector3 mCurrentDirection;
		const double mRotationSpeed = 120.0f;			// In degrees / s
		std::vector<std::string> mCombatMoves;
	public:
		SimplePlayer(std::string name, unsigned int tag);
		virtual ~SimplePlayer();

		virtual void handleAction(std::string action, bool isStarting) override;

		//virtual void update(double dt) override;
	};

}

#endif