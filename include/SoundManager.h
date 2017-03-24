#ifndef _SOUND_MANAGER_H_
#define _SOUND_MANAGER_H_

#include <stdafx.h>
#include <Singleton.h>
#include <GameObject.h>

#define WORLD_UNITS_PER_METER 1.5f
#define METERS_PER_WORLD_UNIT 1.0f / WORLD_UNITS_PER_METER

// Convenience macro to make accessing the SoundManager singleton a bit less cumbersome
#define SOUND_MANAGER ssurge::SoundManager::getSingletonPtr()

namespace ssurge
{
	class ReverbZoneComponent;

	/// This class manages all sounds in the game
	class SoundManager : public Singleton < SoundManager >
	{
	/***** ATTRIBUTES *****/
	private:
		///The IrrKlangEngine that handles all the sounds
		irrklang::ISoundEngine * mEngine;

		///The Game Object that the Listener is attached to 
		ssurge::GameObject * mListenerObject;

		///A map of all the SoundSources as well as their names
		std::map<std::string, irrklang::ISoundSource *> mSources;

		///The volume of the sound 0 to 1
		float mVolume;

		///A boolean that represents if a sound is muted or not
		bool mMuted;

		///A boolean that represents if Reverb is enabled
		bool mReverbEnabled;

		///The volume of reverb
		float mReverbMix;

		///How long it takes the reverb to decay
		float mReverbDecay;

		///Our baseline reverb mix
		float mDefaultReverbMix;

		///Our baseline reverb decay time
		float mDefaultReverbDecay;

		///A vector that holds all of our reverb zones
		std::vector<ReverbZoneComponent *> mReverbZones;

		///The position of the listener
		Ogre::Vector3 mPos;

	/***** CONSTRUCTORS / DESTRUCTORS *****/
	public:
		/// One and only (singleton) constructor
		SoundManager();

		/// Destructor
		virtual ~SoundManager();

	/***** METHODS *****/
	private:
		///Sets the Listener position as well as their direction and velocity (for the doppler effect)
		void setListenerPosition(Ogre::Vector3 position, Ogre::Quaternion dir, Ogre::Vector3 velocity);

		///Converts decibel volume (-96 to 0) to linear volume (0 to 1)
		float decibelsToLinear(float db);

		///Converts linear volume (0 to 1) to decibel volume (-96 to 0)
		float linearToDecibels(float lin);

	public:
		///Set the GameObject to be our listener
		void setListenerGameObject(ssurge::GameObject * object);

		///Creates our SoundSource before it needs to be played
		void preloadSound(const char * fname);

		///Returns a SoundSource based on it's name
		irrklang::ISoundSource * getSound(const char * fname);

		///Helper function that takes a Orge Vector3 and makes a IrrKlang Vec3df
		irrklang::vec3df OgreVecToVec3df(Ogre::Vector3 v) { return irrklang::vec3df(v.x, v.y, v.z); }

		///Creates the ISound and begins to play the sound
		irrklang::ISound * playSound(irrklang::ISoundSource * source, Ogre::Vector3 pos,
				Ogre::Vector3 vel = Ogre::Vector3::ZERO, float vol = 1.0f, bool isLooped = false, float minDist = 1.0f);

		///Creates and plays a ISound that does need a position and can be heard throughout the entire area
		irrklang::ISound * playSound2d(irrklang::ISoundSource * source, float vol = 1.0f, bool isLooped = false);

		///Removes a specfic SoundSource and destroys it
		void removeSoundSource(irrklang::ISoundSource * source);

		///Adds a reverb zone to mReverbZones
		void addReverbZone(ReverbZoneComponent * component);

		///Removes a reverb zone from mReverbZones
		void removeReverbZone(ReverbZoneComponent * component);

		///Updates listener information and recalculates reverb
		void update(double dt);

	/***** GETTERS / SETTERS *****/
	public:
		///Gets the reverb mix
		float getReverbMix() { return mReverbEnabled ? mReverbMix : -96.0f; }

		///Gets the reverb decay time
		float getReverbDecay() { return mReverbDecay; }

		///Sets the baseline reverb mix
		void setDefaultReverbMix(float mix) { mDefaultReverbMix = mix; }

		///Sets the baseline reverb decay time
		void setDefaultReverbDecay(float decay) { mDefaultReverbDecay = decay; }

		///Sets the overall volume
		void setVolume(float vol) { mVolume = vol; }

		///Sets whether the engine is muted or not
		void setIsMuted(bool isMuted) { mMuted = isMuted; }

		///Enables reverb
		void enableReverb() { mReverbEnabled = true; }

		///Disables reverb
		void disableReverb() { mReverbEnabled = false; }
	};

}

#endif