#include <stdafx.h>
#include <SoundManager.h>
#include <Exception.h>
#include <stdio.h>
#include <conio.h>
#include <ReverbZoneComponent.h>

// Template specialization to initialize singleton attribute
template<>
ssurge::SoundManager * ssurge::Singleton<ssurge::SoundManager>::msSingleton = NULL;

ssurge::SoundManager::SoundManager() : mDefaultReverbDecay(0), mDefaultReverbMix(0), mListenerObject(NULL), 
		mReverbEnabled(false), mMuted(false), mVolume(1.0f)
{
	mEngine = irrklang::createIrrKlangDevice();
	if (!mEngine)
		new Exception(__FILE__, __LINE__, "Error in initializing sound engine");
	mEngine->loadPlugins("");
	setListenerPosition(Ogre::Vector3::ZERO, Ogre::Quaternion::ZERO, Ogre::Vector3::ZERO);
	mEngine->setDopplerEffectParameters(1.0f, METERS_PER_WORLD_UNIT);
}

ssurge::SoundManager::~SoundManager()
{
	mEngine->removeAllSoundSources();
	if (mEngine)
		mEngine->drop();
	mEngine = nullptr;
}

void ssurge::SoundManager::preloadSound(const char * fname)
{
	mSources[fname] = mEngine->addSoundSourceFromFile(fname, irrklang::ESM_AUTO_DETECT, true);
}

irrklang::ISoundSource * ssurge::SoundManager::getSound(const char * fname)
{
	if (mSources[fname])
		return mSources[fname];
	return mEngine->addSoundSourceFromFile(fname);
}

void ssurge::SoundManager::removeSoundSource(irrklang::ISoundSource * source)
{
	for (std::map<std::string, irrklang::ISoundSource *>::iterator it = mSources.begin(); it != mSources.end(); ++it)
		if (it->second == source)
			return;
	mEngine->removeSoundSource(source);
}

irrklang::ISound * ssurge::SoundManager::playSound(irrklang::ISoundSource * source, Ogre::Vector3 pos, 
			Ogre::Vector3 vel, float vol, bool isLooped, float minDist)
{
	irrklang::ISound * sound = mEngine->play3D(source, OgreVecToVec3df(pos), isLooped, true, true, true);
	if (!sound)
		new Exception(__FILE__, __LINE__, "could not play file");
	sound->setVelocity(OgreVecToVec3df(pos));
	sound->setMinDistance(minDist);
	sound->setVolume(vol);
	sound->setIsPaused(false);
	return sound;
}

irrklang::ISound * ssurge::SoundManager::playSound2d(irrklang::ISoundSource * source, float vol, bool isLooped)
{
	irrklang::ISound * sound = mEngine->play2D(source, isLooped, true, true, false);
	sound->setVolume(vol);
	sound->setIsPaused(false);
	return sound;
}

void ssurge::SoundManager::setListenerGameObject(ssurge::GameObject * object)
{
	mListenerObject = object;
}

void ssurge::SoundManager::setListenerPosition(Ogre::Vector3 position, Ogre::Quaternion dir, Ogre::Vector3 velocity)
{
	mPos = position;
	Ogre::Vector3 _dir = dir.yAxis();
	mEngine->setListenerPosition(OgreVecToVec3df(position), 
			OgreVecToVec3df(_dir), OgreVecToVec3df(velocity), OgreVecToVec3df(Ogre::Vector3::UNIT_Z));
	//std::cout << _dir.x << " " << _dir.y << " " << _dir.z << "\n";
}

void ssurge::SoundManager::addReverbZone(ReverbZoneComponent * component)
{
	mReverbZones.push_back(component);
}

void ssurge::SoundManager::removeReverbZone(ReverbZoneComponent * component)
{
	for (std::vector<ReverbZoneComponent *>::iterator it = mReverbZones.begin(); it != mReverbZones.end();)
	{
		if ((*it) == component)
			it = mReverbZones.erase(it);
		else
			it++;
	}
}

float ssurge::SoundManager::decibelsToLinear(float db)
{
	return std::powf(db, 10.0f);
}

float ssurge::SoundManager::linearToDecibels(float lin)
{
	return std::max(-96.0f, std::log10f(lin));
}

void ssurge::SoundManager::update(double dt)
{
	mEngine->update();

	//Update volume
	mEngine->setSoundVolume(mMuted ? 0.0f : mVolume);

	//Update listener position/direction/velocity
	static Ogre::Vector3 lastPos;
	static bool useLastPos;
	if (mListenerObject)
	{
		Ogre::Vector3 pos = mListenerObject->mSceneNode->_getDerivedPosition();
		Ogre::Quaternion dir = mListenerObject->mSceneNode->_getDerivedOrientation();
		Ogre::Vector3 vel = useLastPos ? (METERS_PER_WORLD_UNIT * (pos - lastPos) / (float)dt) : Ogre::Vector3::ZERO;
		//std::cout << vel.x << " " << vel.y << " " << vel.z << "\n";
		lastPos = pos;
		useLastPos = true;
		setListenerPosition(pos, dir, vel);
	}
	else
		useLastPos = false;

	//Update reverb values
	if (mReverbEnabled)
	{
		float sum = 0.0f;
		for (std::vector<ReverbZoneComponent *>::iterator it = mReverbZones.begin(); it != mReverbZones.end(); ++it)
			sum += (*it)->getInfluence(mPos);
		float d = std::max(0.0f, 1.0f - sum);

		float decay = d * mDefaultReverbDecay;
		for (std::vector<ReverbZoneComponent *>::iterator it = mReverbZones.begin(); it != mReverbZones.end(); ++it)
			decay += (*it)->getInfluence(mPos) * (*it)->getDecayTime();
		decay /= (sum + d);
		mReverbDecay = decay;

		float mix = d * mDefaultReverbMix;
		for (std::vector<ReverbZoneComponent *>::iterator it = mReverbZones.begin(); it != mReverbZones.end(); ++it)
			mix += (*it)->getInfluence(mPos) * (*it)->getMix();
		mix /= (sum + d);
		mReverbMix = linearToDecibels(mix);

		//std::cout << mReverbMix << " " << mReverbDecay << "\n";
	}

	
}