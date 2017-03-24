#ifndef _FULLSCREEN_SHADER_MANAGER_H_
#define _FULLSCREEN_SHADER_MANAGER_H_

#include <stdafx.h>
#include <Singleton.h>

namespace ssurge
{
	class FullscreenShaderManager : public Singleton<FullscreenShaderManager>
		, public Ogre::CompositorLogic,   public Ogre::CompositorInstance::Listener		
	{
	protected:
		float mTexWeights[15][4];
		float mTexOffsetsHorz[15][4];
		float mTexOffsetsVert[15][4];
	public:
		FullscreenShaderManager();
		virtual ~FullscreenShaderManager();
		virtual void compositorInstanceCreated(Ogre::CompositorInstance *newInstance) override;
		virtual void compositorInstanceDestroyed(Ogre::CompositorInstance *destroyedInstance) override;
		virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat) override;
		virtual void notifyViewportSize(int width, int height);
	};

}

#endif

