#ifndef _INPUT_COMPONENT_H_
#define _INPUT_COMPONENT_H_
#include <stdafx.h>
#include <Component.h>
#include <InputManager.h>
namespace ssurge
{

	class GameObject;

	class InputComponent : public Component
	{
	protected:
		//@KJ: I don't think we need the following values -- those were only necessary for the SimplePlayer.  Think in more general terms for this component.
		std::vector<float> mOrbSpeeds;
		std::vector<GameObject*> mOrbs;
		unsigned int mNextOrbID;
		//@KJ: This is the type of thing I *do* want to see in this component.  Talk to Eddie -- we may want to move his axes here.
		double mSpeed = 2.0f;
		Ogre::Vector3 mCurrentDirection;
		//@KJ: This is a keeper too.
		const double mRotationSpeed = 120.0f;
	public:
		InputComponent(GameObject * owner);
		~InputComponent();
		/***** METHODS *****/
	public:
		
		/// This method should be called once per frame
		virtual void update(double dt);
	};
}
#endif
