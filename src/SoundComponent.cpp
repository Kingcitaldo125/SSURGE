#include <stdafx.h>
#include <SoundComponent.h>
#include <Application.h>


ssurge::SoundComponent::SoundComponent(ssurge::GameObject * owner, const char * fname, bool is2D) : Component(owner), mISound(NULL)
{
	mSoundSource = SOUND_MANAGER->getSound(fname);
	if (is2D != true)
	{
		mNode = mOwner->getSceneNode();
		Ogre::Vector3 soundPos = mNode->_getDerivedPosition();
		irrklang::vec3df irSoundPos = SOUND_MANAGER->OgreVecToVec3df(soundPos);
	}
	mIs2D = is2D;

	//Register all methods
	registerOperation("playSound", ComponentType::SOUND);
	registerOperation("pauseSound", ComponentType::SOUND);
	registerOperation("unpauseSound", ComponentType::SOUND);
	registerOperation("stopSound", ComponentType::SOUND);
	registerOperation("setVolume", ComponentType::SOUND);
}

ssurge::Tuple * ssurge::SoundComponent::handleOperation(std::string o, Tuple * t)
{
	if (o == "playSound")
	{
		if (t->getSize() == 0)
			playSound();
		if (t->getSize() == 1)
			playSound(t->getItemAsFloat(0));
		if (t->getSize() == 2)
			playSound(t->getItemAsFloat(0), t->getItemAsBool(1));
		if (t->getSize() >= 3)
			playSound(t->getItemAsFloat(0), t->getItemAsBool(1), t->getItemAsFloat(2));
	}
	else if (o == "pauseSound")
		pauseSound();
	else if (o == "unpauseSound")
		unpauseSound();
	else if (o == "stopSound")
		stopSound();
	else if (o == "setVolume")
		setVolume(t->getItemAsFloat(0));

	return nullptr;
}

ssurge::SoundComponent::~SoundComponent()
{

	if (mISound)
	{
		mISound->stop();
		mISound->drop();
	}
	SOUND_MANAGER->removeSoundSource(mSoundSource);
}

irrklang::ISound * ssurge::SoundComponent::playSound(float volume, bool mIsLooped, float mMinDist)
{
	if (mISound)
	{
		mISound->stop();
		mISound->drop();
	}
	if (mIs2D == false)
		mISound = SOUND_MANAGER->playSound(mSoundSource, soundPos, Ogre::Vector3::ZERO, volume, mIsLooped, mMinDist);
	else
		mISound = SOUND_MANAGER->playSound2d(mSoundSource, volume, mIsLooped);
	return mISound;
}

void ssurge::SoundComponent::pauseSound()
{
	if (mISound != NULL)
	mISound->setIsPaused(true);
}

void ssurge::SoundComponent::unpauseSound()
{
	if (mISound != NULL)
	mISound->setIsPaused(false);
}

void ssurge::SoundComponent::stopSound()
{
	if (mISound != NULL)
		mISound->stop();
}

void ssurge::SoundComponent::setVolume(float wantedVol)
{
	if (mISound != NULL)
		SOUND_MANAGER->setVolume(wantedVol);
}

void ssurge::SoundComponent::update(double dt)
{
	if (mISound && !mIs2D)
	{
		soundPos = mNode->_getDerivedPosition();
		mISound->setPosition(SOUND_MANAGER->OgreVecToVec3df(soundPos));
		Ogre::Vector3 velVec = useLastPos ? (METERS_PER_WORLD_UNIT * (soundPos - lastPos) / (float)dt) : Ogre::Vector3::ZERO;
		lastPos = soundPos;
		useLastPos = true;
		mISound->setVelocity(SOUND_MANAGER->OgreVecToVec3df(velVec));
		mISound->getSoundEffectControl()->enableWavesReverbSoundEffect(0.0f, SOUND_MANAGER->getReverbMix(), SOUND_MANAGER->getReverbDecay());
	}
	else
		useLastPos = false;
}

bool ssurge::SoundComponent::hasPlayingSound()
{
	return mISound;
}

irrklang::ISoundEffectControl * ssurge::SoundComponent::getEffectControl()
{
	return mISound ? mISound->getSoundEffectControl() : nullptr;
}