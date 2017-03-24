#include <stdafx.h>
#include <FullscreenShaderManager.h>
#include <Application.h>

// Template specialization to initialize the singleton.
template<> ssurge::FullscreenShaderManager * ssurge::Singleton<ssurge::FullscreenShaderManager>::msSingleton = NULL;

ssurge::FullscreenShaderManager::FullscreenShaderManager()
{
	// Set up the gaussian support systems
	Ogre::CompositorManager::getSingleton().registerCompositorLogic("GaussianBlur", this);
}


ssurge::FullscreenShaderManager::~FullscreenShaderManager()
{
	Ogre::CompositorManager::getSingleton().unregisterCompositorLogic("GaussianBlur");
}


void ssurge::FullscreenShaderManager::compositorInstanceCreated(Ogre::CompositorInstance *newInstance)
{
	newInstance->addListener(this);
	Ogre::Viewport * vp = newInstance->getChain()->getViewport();
	notifyViewportSize((int)vp->getActualWidth(), (int)vp->getActualHeight());
}

void ssurge::FullscreenShaderManager::compositorInstanceDestroyed(Ogre::CompositorInstance *destroyedInstance)
{
	destroyedInstance->removeListener(this);
}


void ssurge::FullscreenShaderManager::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	// Prepare the fragment params offsets
	switch (pass_id)
	{
		case 701: // blur horz
		{
			// horizontal bloom
			mat->load();
			Ogre::GpuProgramParametersSharedPtr fparams =
				mat->getBestTechnique()->getPass(0)->getFragmentProgramParameters();
			fparams->setNamedConstant("sampleOffsets", mTexOffsetsHorz[0], 15);
			fparams->setNamedConstant("sampleWeights", mTexWeights[0], 15);

			break;
		}
		case 700: // blur vert
		{
			// vertical bloom
			mat->load();
			Ogre::GpuProgramParametersSharedPtr fparams =
				mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
			fparams->setNamedConstant("sampleOffsets", mTexOffsetsVert[0], 15);
			fparams->setNamedConstant("sampleWeights", mTexWeights[0], 15);

			break;
		}
	}
}


void ssurge::FullscreenShaderManager::notifyViewportSize(int width, int height)
{
	// Calculate gaussian texture offsets & weights
	float deviation = 3.0f;
	float texelSize = 1.0f / (float)std::min((float)width, float(height));

	// central sample, no offset
	mTexOffsetsHorz[0][0] = 0.0f;
	mTexOffsetsHorz[0][1] = 0.0f;
	mTexOffsetsVert[0][0] = 0.0f;
	mTexOffsetsVert[0][1] = 0.0f;
	mTexWeights[0][0] = mTexWeights[0][1] =
	mTexWeights[0][2] = Ogre::Math::gaussianDistribution(0, 0, deviation);
	mTexWeights[0][3] = 1.0f;

	// 'pre' samples
	for (int i = 1; i < 8; ++i)
	{
		mTexWeights[i][0] = mTexWeights[i][1] =
		mTexWeights[i][2] = Ogre::Math::gaussianDistribution((Ogre::Real)i, 0, deviation);
		mTexWeights[i][3] = 1.0f;
		mTexOffsetsHorz[i][0] = i * texelSize;
		mTexOffsetsHorz[i][1] = 0.0f;
		mTexOffsetsVert[i][0] = 0.0f;
		mTexOffsetsVert[i][1] = i * texelSize;
	}
	// 'post' samples
	for (int i = 8; i < 15; ++i)
	{
		mTexWeights[i][0] = mTexWeights[i][1] =
		mTexWeights[i][2] = mTexWeights[i - 7][0];
		mTexWeights[i][3] = 1.0f;

		mTexOffsetsHorz[i][0] = -mTexOffsetsHorz[i - 7][0];
		mTexOffsetsHorz[i][1] = 0.0f;
		mTexOffsetsVert[i][0] = 0.0f;
		mTexOffsetsVert[i][1] = -mTexOffsetsVert[i - 7][1];
	}
}