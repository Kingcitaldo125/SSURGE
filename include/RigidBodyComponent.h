#pragma once
#include <stdafx.h>
#include <btBulletDynamicsCommon.h>
#include <Component.h>

namespace ssurge
{

	class GameObject;

	///A container class that contains a refernce to a bullet rigidbody that allows for physics simulation and gameobject position updating
	class RigidBodyComponent : public Component
	{
	// ***** MEMBERS ***** //
	protected:
		btRigidBody::btRigidBodyConstructionInfo * mConstructionInfo;

		btRigidBody * mRigidBody;

		
	// ***** CONSTRUCTORS / DECONSTRUCTORS ***** //
	public:
		RigidBodyComponent(ssurge::GameObject * owner);

		RigidBodyComponent(ssurge::GameObject * owner, btCollisionShape * shape, btScalar mass);

		RigidBodyComponent(ssurge::GameObject * owner, RigidBodyComponent * rigidBody);

		RigidBodyComponent(ssurge::GameObject * owner, btRigidBody::btRigidBodyConstructionInfo * constructionInfo);
		
		~RigidBodyComponent();
		
	// ***** MEMBER FUNCTIONS ***** //
		void init(ssurge::GameObject * owner, btRigidBody::btRigidBodyConstructionInfo * constructionInfo);

		///Returns the type of this component
		ComponentType getType() override { return ComponentType::RIGIDBODY; }

		///Gets a reference to the containing Bullet RigidBody
		btRigidBody * getRigidBody() { return mRigidBody; }

		///Gets the containing Bullet RigidBody's ConstructionInfo for use of rigidbody cloning
		btRigidBody::btRigidBodyConstructionInfo  * getConstructionInfo() { return mConstructionInfo; }

		///Updates the position of the attacthed gameobject to that of the bullet rigidbody in the dynamcis world
		virtual void update(double dTime) override;

		/// Regesters all the methods of this component
		void registerAll();
		/// sets the restitution
		void setRestitution(float bounce);
		/// sets the friction
		void setFriction(float friction);
		/// sets the rolling friction
		void setRollingFriction(float frictionRol);
		/// handles operation givent to it from the outside
		ssurge::Tuple * handleOperation(std::string method, ssurge::Tuple * tuple) override;
	};

}