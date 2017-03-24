#ifndef _SOUND_COMPONENT_H_
#define _SOUND_COMPONENT_H_

#include <stdafx.h>
#include <Component.h>
#include <Tuple.h>

namespace ssurge
{
	class GameObject;

	/// SoundComponents are responsible for loading a Sound, updating its position if necessary
	class SoundComponent : public Component
	{
	/***** ATTRIBUTES *****/
	public:
		///The input file and some of its settings. It allows us to put soundeffects onto our sound
		irrklang::ISoundSource * mSoundSource;

		///The Ogre SceneNode that out GameObject is attached to (allows us to get out the position)
		Ogre::SceneNode * mNode;

		///The position of our GameObject and in turn, our sound (if 3D, 2D required no position)
		Ogre::Vector3 soundPos;

		///The actual sound being played, read in by Irrklang from our SoundSource
		irrklang::ISound * mISound;

		///A boolean that says if this sound is 2D or 3D (able to be heard everywhere, regardless of 
		///direction, such as background music, or if it's tied to a specific object, such as an animal's footsteps)
		///Regardless of 2D or 3D, there can only be one sound per GameObject.
		bool mIs2D;
		
	private:
		///Previous frame's location, used to calculate velocity
		Ogre::Vector3 lastPos;
		
		///Used to determine whether or not the previous position is valid and should be used
		bool useLastPos;

	/***** CONSTRUCTORS / DESTRUCTORS *****/
	public:
		///Creates the SoundSource but not the sound, playSound must be called in order to create the ISound
		SoundComponent(GameObject * owner, const char * fname, bool is2D = false);

		///Removes the SoundSource
		~SoundComponent();

		/// Says what ComponentType we are (Hint:Its SOUND)
		ComponentType getType() override { return ComponentType::SOUND; }

	/***** METHODS *****/
	public:
		///This allows the sound to be played as well as creating the ISound
		irrklang::ISound * playSound(float volume = 1.0f, bool mIsLooped = false, float mMinDist = 1.0);

		///Pauses the sound obviously
		void pauseSound();

		///Resumes the sound at the point that it was paused at
		void unpauseSound();

		///Updates the sound position as well as its velocity and reverb mix (only really necessary for 3D)
		void update(double dt);

		///Stops the sound and destroy it
		void stopSound();

		///Set the volume of the sound from 0 to 1
		void setVolume(float wantedVol);

		/// Forwarded by the owner GameObject from the Network Manager when it is instructed by an incoming packet to handle an operation
		Tuple * handleOperation(std::string o, Tuple * t);

	/***** GETTERS / SETTERS *****/
		///Returns whether an active sound exists
		bool hasPlayingSound();

		///Returns the active sound's effect control if it exists, and can be used to apply sound effects to the component.
		///Documentation for these effects can be found here http://www.ambiera.com/irrklang/docu/classirrklang_1_1_i_sound_effect_control.html
		///Reverb set with this will automatically be overwritten.
		irrklang::ISoundEffectControl * getEffectControl();
	};
}

#endif