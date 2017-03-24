#include <stdafx.h>
#include <MeshComponent.h>
#include <Application.h>


ssurge::MeshComponent::MeshComponent(ssurge::GameObject * owner) : Component(owner), mEntity(NULL) 
{
	// Stick us in the proper "slot" of owner.  I'd *like* to do this in Component
	// (because all Component-derived classes will need to this).  But I can't 
	// because addComponent needs to have a fully-working getType() method (so 
	// addComponent can determine the proper slot).
	mOwner->addComponent(this);
	registerOperation("loadMesh", ComponentType::MESH);
	registerOperation("unloadMesh", ComponentType::MESH);
	registerOperation("setMaterial", ComponentType::MESH);
	registerOperation("addAnimSwitchType", ComponentType::MESH);
	registerOperation("addBoneMask", ComponentType::MESH);
	registerOperation("queueAnim", ComponentType::MESH);
	registerOperation("playAnim", ComponentType::MESH);
	registerOperation("setDefaultAnim", ComponentType::MESH);
	registerOperation("playDefaultAnim", ComponentType::MESH);
}


ssurge::MeshComponent::~MeshComponent()
{
	if (mEntity != NULL)
		unloadMesh();
}

ssurge::Tuple * ssurge::MeshComponent::handleOperation(std::string o, ssurge::Tuple * t)
{
	if (o == "loadMesh")
	{
		loadMesh(t->getItemAsString(0));
	}
	else if (o == "unloadMesh")
	{
		unloadMesh();
	}
	else if (o == "setMaterial")
	{
		setMaterial(t->getItemAsInt(0), t->getItemAsString(1));
	}
	else if (o == "addAnimSwitchType")
	{
		std::string tString = t->getItemAsString(2);
		AnimationBlender::BlendingTransition transition;
		if (tString == "BlendSwitch")
		{
			transition = AnimationBlender::BlendingTransition::BlendSwitch;
		}
		else if (tString == "BlendThenAnimate")
		{
			transition = AnimationBlender::BlendingTransition::BlendThenAnimate;
		}
		else if (tString == "BlendWhileAnimating")
		{
			transition = AnimationBlender::BlendingTransition::BlendWhileAnimating;
		}
		else
		{
			mOwner->signalOperationFailure();
			LOG_MANAGER->log("Incorrect Blending Transition", LL_ERROR);
			return NULL;
		}
		addAnimSwitchType(t->getItemAsString(0), t->getItemAsString(1), transition);
	}
	else if (o == "addBoneMask")
	{
		std::vector<int> v;
		for (int i = 1; i != t->getSize(); ++i)
		{
			v.push_back(t->getItemAsInt(i));
		}
		addBoneMask(t->getItemAsString(0),v);
	}
	else if (o == "queueAnim")
	{
		if (t->getSize() == 3)
		{
			queueAnim(t->getItemAsString(0), t->getItemAsBool(1), t->getItemAsString(2));
		}
		else
			queueAnim(t->getItemAsString(0), t->getItemAsBool(1));
	}
	else if (o == "playAnim")
	{
		if (t->getSize() == 3)
		{
			playAnim(t->getItemAsString(0), t->getItemAsBool(1), t->getItemAsString(2));
		}
		else
			playAnim(t->getItemAsString(0), t->getItemAsBool(1));
	}
	else if (o == "setDefaultAnim")
	{
		setDefaultAnim(t->getItemAsString(0));
	}
	else if (o == "playDefaultAnim")
	{
		playDefaultAnim();
	}
	return NULL;
}


void ssurge::MeshComponent::update(double dt) 
{ 
	if (mNumAnimations > 0)
		animate(dt);
}


void ssurge::MeshComponent::unloadMesh()
{
	if (mEntity != NULL)
	{
		Ogre::SceneNode * snode = mEntity->getParentSceneNode();
		if (snode != NULL)
			snode->detachObject(mEntity);
		APPLICATION->getSceneManager()->destroyEntity(mEntity);
		mEntity = NULL;
	}
}

		
void ssurge::MeshComponent::loadMesh(std::string fname)
{
	if (mEntity)
		unloadMesh();

	mEntity = APPLICATION->getSceneManager()->createEntity(mOwner->getName() + "_mesh", fname);
	mSkeleton = mEntity->getSkeleton();
	if (mSkeleton != nullptr)
	{
		mNumAnimations = mSkeleton->getNumAnimations();
		std::ofstream animList;
		animList.open("..//media//" + mOwner->getName() + " Animation List.txt");
		//Ogre::AnimationStateSet * animSet = mEntity->getAllAnimationStates();
		Ogre::AnimationStateIterator iter = mEntity->getAllAnimationStates()->getAnimationStateIterator();
		for (iter.begin(); iter.current() != iter.end(); iter.getNext())
		{
			animList << iter.current()->first.c_str() << std::endl;
		}
		animList << std::endl;
		int numBones = mSkeleton->getNumBones();
		for (int i = 0; i < numBones; i++)
		{
			animList << i << " " << mSkeleton->getBone(i)->getName() << std::endl;
		}
	}

	mAnimBlender = new AnimationBlender(mEntity);
	if(mOwner->getName() != "Floor")
		mOwner->getSceneNode()->attachObject(mEntity);
}

void ssurge::MeshComponent::setMaterial(int subMeshIdx, std::string matName)
{
	mEntity->getSubEntity(subMeshIdx)->setMaterialName(matName);
}

void ssurge::MeshComponent::addAnimSwitchType(std::string srcAnim, std::string destAnim, AnimationBlender::BlendingTransition transitionType)
{
	mBlendTypes[srcAnim + "->" + destAnim] = transitionType;
}

void ssurge::MeshComponent::addBoneMask(std::string name, std::vector<int> vec)
{
	mAnimLocations[name] = vec;
}

void ssurge::MeshComponent::queueAnim(std::string animName, bool loop, std::string blendMask)
{
	if (mNumAnimations)
	{
		Ogre::AnimationState * animState = mEntity->getAnimationState(animName);
		if (blendMask != "")
		{
			animState->createBlendMask(mSkeleton->getNumBones(), 0.0);
			std::vector<int> v = mAnimLocations.at(blendMask);
			if (v.size() != 0)
			{
				for (int i : v)
				{
					animState->setBlendMaskEntry(i, 1);
				}
			}
		}
		animState->setLoop(loop);
		mAnimQueue.push(animState);
	}
}

void ssurge::MeshComponent::playAnim(std::string animName, bool loop, std::string blendMask)
{
	if (mNumAnimations)
	{
		std::string prevAnim = "";
		if (!mAnimQueue.empty())
		{
			mAnimQueue.front()->setTimePosition(0.0);
			prevAnim = mAnimQueue.front()->getAnimationName();
			Ogre::AnimationStateIterator iter = mEntity->getAllAnimationStates()->getAnimationStateIterator();
			for (iter.begin(); iter.current() != iter.end(); iter.getNext())
			{
				if (iter.current()->second->hasBlendMask())
				{
					iter.current()->second->destroyBlendMask();
				}
			}
			mAnimQueue = decltype(mAnimQueue)();
		}
		Ogre::AnimationState * animState = mEntity->getAnimationState(animName);
		mSkeleton->setBlendMode(Ogre::SkeletonAnimationBlendMode::ANIMBLEND_AVERAGE);
		if (blendMask != "")
		{
			mSkeleton->setBlendMode(Ogre::SkeletonAnimationBlendMode::ANIMBLEND_CUMULATIVE);
			animState->createBlendMask(mSkeleton->getNumBones(), 0.0);
			std::vector<int> v = mAnimLocations.at(blendMask);
			if (v.size() != 0)
			{
				for (int i : v)
				{
					animState->setBlendMaskEntry(i, 1);
				}
			}
		}
		animState->setLoop(loop);
		mAnimQueue.push(animState);
		if (mBlendTypes.find(prevAnim + "->" + animName) != mBlendTypes.end())
		{
			mAnimBlender->blend(animName, mBlendTypes.find(prevAnim + "->" + animName)->second, 0.2, loop);
		}
		else
		{
			mAnimBlender->blend(animName, AnimationBlender::BlendWhileAnimating, 0.2, loop);
		}
	}
}

void ssurge::MeshComponent::updateAnimQueue()
{
	std::string prevAnim = "";
	if ((mAnimQueue.size() > 1 && mAnimQueue.front()->getLoop()) || !mAnimQueue.front()->getLoop() && mAnimQueue.front()->hasEnded())
	{
		prevAnim = mAnimQueue.front()->getAnimationName();
		mAnimQueue.pop();
		if (!mAnimQueue.empty())
		{
			if (prevAnim != mAnimQueue.front()->getAnimationName())
			{
				if (mAnimQueue.front()->hasBlendMask())
				{
					mSkeleton->setBlendMode(Ogre::SkeletonAnimationBlendMode::ANIMBLEND_CUMULATIVE);
				}
				if (mBlendTypes.find(prevAnim + "->" + mAnimQueue.front()->getAnimationName()) != mBlendTypes.end())
				{
					mAnimBlender->blend(mAnimQueue.front()->getAnimationName(), mBlendTypes.find(prevAnim + "->" + mAnimQueue.front()->getAnimationName())->second, 0.3, mAnimQueue.front()->getLoop());
				}
				else
				{
					mAnimBlender->blend(mAnimQueue.front()->getAnimationName(), AnimationBlender::BlendWhileAnimating, 0.3, mAnimQueue.front()->getLoop());
				}
			}
			return;
		}
	}
	if (mAnimQueue.empty())
		playDefaultAnim();
}

void ssurge::MeshComponent::setDefaultAnim(std::string animName)
{
	mDefaultAnim = mEntity->getAnimationState(animName);
	mAnimQueue.push(mDefaultAnim);
	mAnimBlender->init(animName, true);
}

void ssurge::MeshComponent::playDefaultAnim()
{
	mDefaultAnim->destroyBlendMask();
	playAnim(mDefaultAnim->getAnimationName(), true);
}

void ssurge::MeshComponent::animate(double dt)
{
	if (mDefaultAnim != nullptr)
	{
		this->updateAnimQueue();
		mAnimBlender->addTime(dt);
	}
	else
	{
		LOG_MANAGER->log("Need to set a default animation for" + mOwner->getName() + ".", LL_ERROR);
	}
}