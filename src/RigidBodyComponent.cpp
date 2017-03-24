#include <stdafx.h>
#include <RigidBodyComponent.h>
#include <Application.h>


ssurge::RigidBodyComponent::RigidBodyComponent(ssurge::GameObject * owner, btCollisionShape * shape, btScalar mass) : Component(owner)
{
	Ogre::Vector3 pos = mOwner->getSceneNode()->getPosition();
	Ogre::Quaternion rot = mOwner->getSceneNode()->getOrientation();
	btTransform t = btTransform(btQuaternion(0.0f,0.0f,0.0f, 1.0f), btVector3((btScalar)pos.x, (btScalar)pos.y, (btScalar)pos.z));
	//btDefaultMotionState * ms = ;
	/*mConstructionInfo = new btRigidBody::btRigidBodyConstructionInfo(mass, ms, shape, btVector3(0, 0, 0));
	mRigidBody = new btRigidBody(*mConstructionInfo);
	mOwner->addComponent(this);
	PHYSICS_MANAGER->addRigidBody(owner->getName(), mRigidBody);*/
	btVector3 inertia = btVector3(0, 0, 0);
	shape->calculateLocalInertia(1, inertia);
	init(owner, new btRigidBody::btRigidBodyConstructionInfo(mass, new btDefaultMotionState(t), shape, inertia));
	this->registerAll();

}

/*ssurge::RigidBodyComponent::RigidBodyComponent(ssurge::GameObject * owner, btCollisionShape * shape, btScalar mass) : Component(owner)
{
	Ogre::Vector3 pos = mOwner->getSceneNode()->getPosition();
	Ogre::Quaternion rot = mOwner->getSceneNode()->getOrientation();
	btTransform t = btTransform(btQuaternion((btScalar)rot.x, (btScalar)rot.y, (btScalar)rot.z, (btScalar)rot.w), btVector3((btScalar)pos.x, (btScalar)pos.y, (btScalar)pos.z));
	btDefaultMotionState * ms = new btDefaultMotionState(t);
	mConstructionInfo = new btRigidBody::btRigidBodyConstructionInfo(mass, ms, shape, btVector3(0,0,0));
	mRigidBody = new btRigidBody(*mConstructionInfo);
	mOwner->addComponent(this);
	PHYSICS_MANAGER->addRigidBody(owner->getName(), mRigidBody);

}*/

ssurge::RigidBodyComponent::RigidBodyComponent(ssurge::GameObject * owner, RigidBodyComponent * rigidBody) : Component(owner)
{
	init(owner, rigidBody->getConstructionInfo());
	/*mConstructionInfo = rigidBody->getConstructionInfo();
	mRigidBody = new btRigidBody(*mConstructionInfo);
	mOwner->addComponent(this);
	PHYSICS_MANAGER->addRigidBody(owner->getName(), mRigidBody);*/
	this->registerAll();
}

ssurge::RigidBodyComponent::RigidBodyComponent(ssurge::GameObject * owner, btRigidBody::btRigidBodyConstructionInfo * constructionInfo) : Component(owner)
{
	/*mConstructionInfo = constructionInfo;
	mRigidBody = new btRigidBody(*mConstructionInfo);
	mOwner->addComponent(this);
	PHYSICS_MANAGER->addRigidBody(owner->getName(), mRigidBody);*/
	init(owner, constructionInfo);
	this->registerAll();
}


void ssurge::RigidBodyComponent::init(ssurge::GameObject * owner, btRigidBody::btRigidBodyConstructionInfo * constructionInfo)
{
	mConstructionInfo = constructionInfo;
	mRigidBody = new btRigidBody(*mConstructionInfo);
	mOwner->addComponent(this);
	PHYSICS_MANAGER->addRigidBody(owner->getName(), mRigidBody);
}


ssurge::RigidBodyComponent::~RigidBodyComponent()
{
 	delete(mRigidBody);
	delete(mConstructionInfo);
}

void ssurge::RigidBodyComponent::registerAll()
{
	this->registerOperation("updateRigid", ssurge::ComponentType::RIGIDBODY);
	this->registerOperation("getTypeRigid", ssurge::ComponentType::RIGIDBODY);
	this->registerOperation("getRigidBodyRigid", ssurge::ComponentType::RIGIDBODY);
	this->registerOperation("getConstructionInfoRigid", ssurge::ComponentType::RIGIDBODY);
	this->registerOperation("setRestitutionRigid", ssurge::ComponentType::RIGIDBODY);
	this->registerOperation("setFrictionRigid", ssurge::ComponentType::RIGIDBODY);
	this->registerOperation("setRollingFrictionRigid", ssurge::ComponentType::RIGIDBODY);
}

ssurge::Tuple * ssurge::RigidBodyComponent::handleOperation(std::string method, ssurge::Tuple * tuple)
{
	if (method == "updateRigid")
	{
		this->update(tuple->getItemAsDouble(0));
	}
	else if (method == "getTypeRigid")
	{
		return new ssurge::Tuple("i", (int)this->getType());
	}
	else if (method == "setRestitutionRigid")
	{
		this->setRestitution(tuple->getItemAsFloat(0));
	}
	else if (method == "setFrictionRigid")
	{
		this->setFriction(tuple->getItemAsFloat(0));
	}
	else if (method == "setRollingFrictionRigid")
	{
		this->setRollingFriction(tuple->getItemAsFloat(0));
	}
	return NULL;
}

void ssurge::RigidBodyComponent::update(double dTime)
{
	btVector3 pos = mRigidBody->getWorldTransform().getOrigin();
	btQuaternion rot = mRigidBody->getWorldTransform().getRotation();
	//mOwner->setOrientationQuaternion(rot.x(), rot.y(), rot.z(), rot.w());
	mOwner->setPosition(pos.x(), pos.y(), pos.z());
	//mOwner->getSceneNode()->setPosition(pos.x(),pos.y(),pos.z());
	mOwner->getSceneNode()->setOrientation(rot.z(), rot.y(), -rot.x(), rot.w());


	/*float newAngle = mRigidBody->getOrientation().getAngle() + 0.2f * dTime;
	if (newAngle > 3.1456)
	{
		newAngle -= 3.1456;
	}
	if (newAngle < 3.1456)
	{
		newAngle += 3.1456;
	}


	btQuaternion q = btQuaternion(mRigidBody->getOrientation().getAxis() + btVector3(0.1f, 0.0, 0.0), mRigidBody->getOrientation().getAngle() + 0.2f * dTime);
	mRigidBody->setWorldTransform(btTransform(q, mRigidBody->getWorldTransform().getOrigin()));*/
}
void ssurge::RigidBodyComponent::setRestitution(float bounce)
{
	this->getRigidBody()->setRestitution(bounce);
}
void ssurge::RigidBodyComponent::setFriction(float friction)
{
	this->getRigidBody()->setFriction(friction);
}
void ssurge::RigidBodyComponent::setRollingFriction(float frictionRol)
{
	this->getRigidBody()->setRollingFriction(frictionRol);
}
