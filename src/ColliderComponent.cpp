#include <stdafx.h>
#include <ColliderComponent.h>
#include <Application.h>
#include <MeshComponent.h>
#include <Tuple.h>
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"





ssurge::ColliderComponent::ColliderComponent(ssurge::GameObject * owner, btCollisionShape * shape) : Component(owner)
{
	// attach itself to its owner, and adds the shape to the physics manager
	mCollisionObject = new btCollisionObject();
	mCollisionShape = shape;
	mCollisionObject->setCollisionShape(mCollisionShape);
	mOwner->addComponent(this);
	PHYSICS_MANAGER->addCollider(owner->getName(), mCollisionShape);
	this->regesterAll();
}

ssurge::ColliderComponent::ColliderComponent(ssurge::GameObject * owner, std::string shape) : Component(owner)
{
	// attach itself to its owner, and adds the shape to the physics manager
	// this constructor makes a shaoe based on the mesh

	mCollisionObject = new btCollisionObject();
	this->changeShapeBasedOnMesh(shape);
	mCollisionObject->setCollisionShape(mCollisionShape);
	mOwner->addComponent(this);
	PHYSICS_MANAGER->addCollider(owner->getName(), mCollisionShape);
	this->regesterAll();
}

ssurge::ColliderComponent::ColliderComponent(ssurge::GameObject * owner, std::string shape,float dim1,float dim2,float dim3,float dim4) : Component(owner)
{
	// attach itself to its owner, and adds the shape to the physics manager
	// this constructor lets you choose the shape and the dimensions

	mCollisionObject = new btCollisionObject();
	this->changeShape(shape,dim1,dim2,dim3,dim4);
	mCollisionObject->setCollisionShape(mCollisionShape);
	mOwner->addComponent(this);
	PHYSICS_MANAGER->addCollider(owner->getName(), mCollisionShape);
	this->regesterAll();
}


ssurge::ColliderComponent::~ColliderComponent()
{
	// calls the destroy on the shape
	mCollisionShape->~btCollisionShape();
	mCollisionObject->~btCollisionObject();
	free(this);
}

void ssurge::ColliderComponent::regesterAll()
{
	this->registerOperation("updateCollider", ssurge::ComponentType::COLLIDER);
	this->registerOperation("getShapeCollider", ssurge::ComponentType::COLLIDER);
	this->registerOperation("getObjectCollider", ssurge::ComponentType::COLLIDER);
	this->registerOperation("moveShapeCollider", ssurge::ComponentType::COLLIDER);
	this->registerOperation("setShapeCollider", ssurge::ComponentType::COLLIDER);
	this->registerOperation("changeShapeBasedOffMeshCollider", ssurge::ComponentType::COLLIDER);
	this->registerOperation("getTypeCollider", ssurge::ComponentType::COLLIDER);
	this->registerOperation("setLocalScalingCollider", ssurge::ComponentType::COLLIDER);
}

ssurge::Tuple* ssurge::ColliderComponent::handleOperation(std::string method, ssurge::Tuple* tuple)
{
	if (method == "updateCollider")
	{
		// does nothing
	}
	else if (method == "getShapeCollider")
	{
		this->getShape();
	}
	else if (method == "getObjectCollider")
	{
		this->getObject();
	}
	else if (method == "moveShapeCollider")
	{
		this->moveShape(Ogre::Quaternion(tuple->getItemAsFloat(0), tuple->getItemAsFloat(1), tuple->getItemAsFloat(2), tuple->getItemAsFloat(3)), Ogre::Vector3(tuple->getItemAsFloat(4), tuple->getItemAsFloat(5), tuple->getItemAsFloat(6)));
	}
	else if (method == "setShapeCollider")
	{
		// need to imlement
	}
	else if (method == "changeShapeBasedOffMeshCollider")
	{
		this->changeShapeBasedOnMesh(tuple->getItemAsString(0));
	}
	else if (method == "getTypeCollider")
	{
		this->getType();
	}
	else if (method == "setLocalScalingCollider")
	{
		this->setLocalScaleing(tuple->getItemAsFloat(0), tuple->getItemAsFloat(1), tuple->getItemAsFloat(2));
	}
	return NULL;
}

void ssurge::ColliderComponent::update(double dt)
{
	// Nothing here -- yet!
}

btCollisionShape* ssurge::ColliderComponent::getShape()
{
	return mCollisionShape;
}
void ssurge::ColliderComponent::setShape(btCollisionShape * shape)
{
	mCollisionShape = shape;
}
void ssurge::ColliderComponent::changeShapeBasedOnMesh(std::string shape)
{
	MeshComponent * temp = static_cast<MeshComponent*>(mOwner->getComponent(ComponentType::MESH));
	if (shape == "sphere")
		mCollisionShape = new btSphereShape(temp->getEntity()->getBoundingRadius());
	else if (shape == "box")
		mCollisionShape = new btBoxShape(btVector3((btScalar)temp->getEntity()->getBoundingBox().getSize().x, (btScalar)temp->getEntity()->getBoundingBox().getSize().y, (btScalar)temp->getEntity()->getBoundingBox().getSize().z));
	else if (shape == "capsule")
		mCollisionShape = new btCapsuleShape(temp->getEntity()->getBoundingRadius(), temp->getEntity()->getBoundingBox().getSize().z);
	//else if (shape == "terrain")
	//mCollisionShape = new btHeightfieldTerrainShape(temp->getEntity()->getBoundingBox().getSize().UNIT_X, temp->getEntity()->getBoundingBox().getSize().UNIT_Y, Data, scale, minHeight, maxHeight, 1, heightDataType, flipQuadEdges);
	else if (shape == "plane")
		mCollisionShape = new btStaticPlaneShape(btVector3(0, 0.5f, 1), 0);
	else if (shape == "cylinder")
		mCollisionShape = new btCylinderShape(btVector3((btScalar)temp->getEntity()->getBoundingBox().getSize().x, (btScalar)temp->getEntity()->getBoundingBox().getSize().y, (btScalar)temp->getEntity()->getBoundingBox().getSize().z));
	else if (shape == "cone")
		this->setShape(new btConeShape(temp->getEntity()->getBoundingRadius(), temp->getEntity()->getBoundingBox().getSize().z));
}
void ssurge::ColliderComponent::changeShape(std::string shape, float dim1, float dim2, float dim3, float dim4)
{
	MeshComponent * temp = static_cast<MeshComponent*>(mOwner->getComponent(ComponentType::MESH));
	if (shape == "sphere")
		mCollisionShape = new btSphereShape(dim1); // sphere takes a radius
	else if (shape == "box")
		mCollisionShape = new btBoxShape(btVector3(dim1,dim2,dim3)); // takes a vector for sides x,y,z
	else if (shape == "capsule")
		mCollisionShape = new btCapsuleShape(dim1,dim2);			// takes a radius and a height
	//else if (shape == "terrain")
	//mCollisionShape = new btHeightfieldTerrainShape(temp->getEntity()->getBoundingBox().getSize().UNIT_X, temp->getEntity()->getBoundingBox().getSize().UNIT_Y, Data, scale, minHeight, maxHeight, 1, heightDataType, flipQuadEdges);
	else if (shape == "plane")
		mCollisionShape = new btStaticPlaneShape(btVector3(dim1,dim2,dim3), dim4); // takes a vector3 for normal, and a d value for distance to 0,0,0
	else if (shape == "cylinder")
		mCollisionShape = new btCylinderShape(btVector3(dim1,dim2,dim3)); // takes a vector3 not sure why?
	else if (shape == "cone")
		mCollisionShape = new btConeShape(dim1,dim2);			// takes a raidus and a height
}
void ssurge::ColliderComponent::setLocalScaleing(float a, float b, float c)
{
	this->mCollisionShape->setLocalScaling(btVector3(a, b, c));
}

btCollisionObject* ssurge::ColliderComponent::getObject()
{
	return mCollisionObject;
}

void ssurge::ColliderComponent::moveShape(Ogre::Quaternion rot, Ogre::Vector3 mov)
{
	btQuaternion mRot = btQuaternion(rot[0], rot[1], rot[2]);
	btVector3 mMov = btVector3(mov[0], mov[1], mov[2]);
	btTransform temp =  btTransform(mRot,mMov);
	mCollisionObject->setWorldTransform(temp);
}

