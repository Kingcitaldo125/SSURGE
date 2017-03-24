#ifndef _MESH_COMPONENT_H_
#define _MESH_COMPONENT_H_

#include <stdafx.h>
#include <Component.h>
#include <AnimationBlender.h>

namespace ssurge
{
	// A forward-reference to the GameObject class (put here to avoid a circular
	// dependency)
	class GameObject;

	/// MeshComponents are responsible for loading a mesh, (eventually) swapping materials,
	/// kinematic animation, etc.
	class MeshComponent : public Component
	{
	/***** ATTRIBUTES *****/
	protected:
		/// The Ogre Mesh Entity (a mesh)
		Ogre::Entity * mEntity;

		/// The default animation state for this mesh
		Ogre::AnimationState * mDefaultAnim;

		/// The skeleton for this mesh
		Ogre::Skeleton * mSkeleton;

		/// The total number of animations for this mesh
		int mNumAnimations = 0;

		/// The queue of animations
		std::queue<Ogre::AnimationState *> mAnimQueue;

		/// The animation blender
		ssurge::AnimationBlender * mAnimBlender;

		/// The map of animation blend types based on src and dest animations
		std::map<std::string, ssurge::AnimationBlender::BlendingTransition> mBlendTypes;

		/// The map of types of animations based on location for the mesh
		std::map<std::string, std::vector<int>> mAnimLocations;

	/***** CONSTRUCTORS / DESTRUCTORS *****/
	public:
		/// Default constructor
		MeshComponent(GameObject * owner);

		/// Destructor
		virtual ~MeshComponent();

	/***** METHODS *****/
	public:
		/// Gets the type of this component
		ComponentType getType() override { return ComponentType::MESH;  }

		/// Updates this component
		virtual void update(double dt) override;

		/// Handle Operation
		virtual Tuple * handleOperation(std::string o, Tuple * t) override;

		/// Unloads the mesh (and destroys the Ogre entity)
		virtual void unloadMesh();

		/// (Re-)loads a mesh and creates a list of animations in the media folder if animations exist for this mesh.
		virtual void loadMesh(std::string fname);

		/// Sets the mesh's material to the new material
		virtual void setMaterial(int subMeshIdx, std::string matName);

		/// Configures the type of blending transition from srcAnim to destAnim
		virtual void addAnimSwitchType(std::string srcAnim, std::string destAnim, AnimationBlender::BlendingTransition transitionType);

		/// Creates a new animation bone mask by passing the name of the bone mask and a vector of bone handles
		virtual void addBoneMask(std::string name, std::vector<int> vec);

		/// Adds an animation to the animation queue
		virtual void queueAnim(std::string animName, bool loop, std::string blendMask = "");

		/// Clears the animation queue and plays the animation immediately
		virtual void playAnim(std::string animName, bool loop, std::string blendMask = "");

		/// Updates the animation queue
		virtual void updateAnimQueue();

		/// Sets the default animation for the mesh
		virtual void setDefaultAnim(std::string animName);

		/// Plays the default animation for the mesh
		virtual void playDefaultAnim();

		/// Animates the first animation in the queue
		virtual void animate(double dt);

		Ogre::Entity * getEntity() { return mEntity; };
	};
}

#endif
