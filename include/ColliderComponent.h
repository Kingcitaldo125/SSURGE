#ifndef _COLLIDER_COMPONENT_H_
#define _COLLIDER_COMPONENT_H_
#include <stdafx.h>
#include <Component.h>
#include <btBulletCollisionCommon.h>

namespace ssurge
{
	// A forward-reference to the GameObject class (put here to avoid a circular
	// dependency)
	class GameObject;

	/// 
	class ColliderComponent : public Component
	{
		/***** ATTRIBUTES *****/
	protected:
		// Shapes used to determine collisions
		btCollisionShape* mCollisionShape;
		// The shape attached to this object to allow us to manually move the shape
		btCollisionObject* mCollisionObject;

		/***** CONSTRUCTORS / DESTRUCTORS *****/
	public:
		/// Default constructor
		ColliderComponent(GameObject * owner, btCollisionShape* shape);
		/// Constuctor builds a sphere or a box based on the bounding ox/spphere of the radius and useing a string to choose between
		ColliderComponent(GameObject * owner, std::string shape = "box");
		/// allows you to give your own dimensions to the shape
		ColliderComponent(GameObject * owner, std::string shape , float dim1 , float dim2=1, float dim3=1, float dim4=1);
		/// Destructor
		virtual ~ColliderComponent();

		/***** METHODS *****/
	public:
		/// Gets the type of this component
		ComponentType getType() override { return ComponentType::COLLIDER; }
		/// Regesters all the methods of this component
		void regesterAll();
		/// handles operation givent to it from the outside
		Tuple* handleOperation(std::string method, Tuple* tuple);
		/// Updates this component
		virtual void update(double dt) override;

		/// returns the collision shape
		btCollisionShape* ssurge::ColliderComponent::getShape();
		/// changes the shape given a shape
		void setShape(btCollisionShape* shape);
		/// changes the shape given a tag to make from the mesh
		void changeShapeBasedOnMesh(std::string shape);
		/// changes the shape given a type and dimensions
		void changeShape(std::string shape, float dim1 = 1, float dim2 =1, float dim3 = 1, float dim4 = 1);
		/// changes scaleimg of a shape
		void setLocalScaleing(float a,float b,float c);
		/// returns the collision object
		btCollisionObject* ssurge::ColliderComponent::getObject();
		/// manually moves the shape if neccesary
		void ssurge::ColliderComponent::moveShape(Ogre::Quaternion rot, Ogre::Vector3 mov);

	};
}

#endif