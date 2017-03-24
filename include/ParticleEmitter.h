#ifndef _PARTICLE_EMITTER_H_
#define _PARTICLE_EMITTER_H_
#include <stdafx.h>
#include <Component.h>



namespace ssurge
{

	class ParticleEmitter : public Component
	{
		public:
			ParticleEmitter(GameObject* owner, std::string partName);
			virtual ~ParticleEmitter();
			ComponentType getType();
			void update(double dt);

		protected:
			Ogre::ParticleSystem* particle_effect;


		/****Attributes****/
		/*protected:
			int angle;
			int emission_rate;
			int time_to_live_min;
			int time_to_live_max;
			int[] direction[3];
			float velocity;
			float[3] colour;
			int width;
			int height;
			int depth;
			float fader_red;
			float fader_green;
			float fader_blue;*/
			

	};






}
#endif