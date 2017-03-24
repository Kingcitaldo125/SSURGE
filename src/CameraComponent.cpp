#include <stdafx.h>
#include <CameraComponent.h>
#include <Application.h>
#include <InputManager.h>
#include <MeshComponent.h>

ssurge::CameraComponent::CameraComponent(ssurge::GameObject * owner, std::vector<ssurge::CameraType> modes) : Component(owner), mCamera(NULL)
{
	mOwner->addComponent(this);
	mName = mOwner->getName() + "Camera";
	mCamera = new Ogre::Camera(mName, APPLICATION->getSceneManager());
	mCamera->setFixedYawAxis(true);
	mCameraType = modes.at(0);
	mCameraModes = modes;
	mSens = 500.0f;
	mYUp = 1.0f;
	mYDown = 0.1f;
	mSens = 500.0f;
	mYUp = 1.0f;
	mYDown = 0.1f;
	mCamSpeed = 25.0f;
	mCamRotSpeed = 0.2f;
	mParentSpeed = 0.02f;
	mAdded = false; mMoving = false;
	mOldPos = mCamera->getPosition();
	mOldOrientation = mCamera->getOrientation();
	mParentForward = ParentForward::NORTH;
	mSwitchMode = false;
}



ssurge::CameraComponent::~CameraComponent()
{
	mOwner->destroyComponent(this->getType());
}



Ogre::Quaternion ssurge::CameraComponent::calculateOrientation(Ogre::Vector3 rotAngle)
{
	if (mCameraType ==CameraType::FIRST_PERSON)
	{
		return mOwner->getSceneNode()->getOrientation();
	}
	else
	{
		Ogre::Quaternion temp = mOwner->getSceneNode()->getOrientation();
		Ogre::Vector3 tempX = temp.xAxis(); tempX.x += rotAngle.x; /// Throw away vectors to set the 
		Ogre::Vector3 tempY = temp.yAxis(); tempY.y += rotAngle.y; /// orientation of our camera equal to the
		Ogre::Vector3 tempZ = temp.zAxis(); tempZ.z += rotAngle.z; /// original orientation + our rotation angle

		return Ogre::Quaternion(tempX, tempY, tempZ);
	}
}



void ssurge::CameraComponent::handleCameraMovement(double dt)
{
	if (INPUT_MANAGER->isKeyDown(OIS::KC_W))
		moveCamera(0.0, 0.0, -1.0, dt);
	if (INPUT_MANAGER->isKeyDown(OIS::KC_S))
		moveCamera(0.0, 0.0, 1.0, dt);
	if (INPUT_MANAGER->isKeyDown(OIS::KC_A))
		moveCamera(-1.0, 0.0, 0.0, dt);
	if (INPUT_MANAGER->isKeyDown(OIS::KC_D))
		moveCamera(1.0, 0.0, 0.0, dt);
}



void ssurge::CameraComponent::handleCameraRotation(std::string axis)
{
	if (axis == "y")
	{
		if (INPUT_MANAGER->isKeyDown(OIS::KC_Q))
			mCamera->yaw(Ogre::Degree(mCamRotSpeed));
		if (INPUT_MANAGER->isKeyDown(OIS::KC_E))
			mCamera->yaw(Ogre::Degree(-mCamRotSpeed));
	}
	else if (axis == "x")
	{
		if (INPUT_MANAGER->isKeyDown(OIS::KC_Q))
			mCamera->pitch(Ogre::Degree(mCamRotSpeed));
		if (INPUT_MANAGER->isKeyDown(OIS::KC_E))
			mCamera->pitch(Ogre::Degree(-mCamRotSpeed));
	}
	else if (axis == "z")
	{
		if (INPUT_MANAGER->isKeyDown(OIS::KC_Q))
			mCamera->roll(Ogre::Degree(mCamRotSpeed));
		if (INPUT_MANAGER->isKeyDown(OIS::KC_E))
			mCamera->roll(Ogre::Degree(-mCamRotSpeed));
	}
	else
		std::cout << "Bad camera rotation axis letter" << std::endl;
}



void ssurge::CameraComponent::moveParentGameObject(double dt)
{
	static float counter = 0.0f;
	static float animCounter = 0.0f;

	if (counter >= 1.0 && INPUT_MANAGER->isKeyDown(OIS::KC_BACK))
	{
		nextMode();
		mSwitchMode = true;
		counter = 0.0f;
	}

	if (INPUT_MANAGER->isKeyDown(OIS::KC_W) || INPUT_MANAGER->isKeyDown(OIS::KC_UP))
	{
		if (mParentForward == ParentForward::WEST)
			mParentGameObject->rotateLocal(45.0, Ogre::Vector3(0.0, -1.0, 0.0));
		if (mParentForward == ParentForward::EAST)
			mParentGameObject->rotateLocal(45.0, Ogre::Vector3(0.0, 1.0, 0.0));
		if (mParentForward == ParentForward::SOUTH)
			mParentGameObject->rotateLocal(90.0, Ogre::Vector3(0.0, 1.0, 0.0));
		mParentForward = ParentForward::NORTH;

		mParentGameObject->translateWorld(Ogre::Vector3(0.0, mParentSpeed, 0.0));

		if (mMoving == false)
		{
			startingAction(false);
			mMoving = true;
		}
		animCounter += 1.0*dt;
	}
	else if (INPUT_MANAGER->isKeyDown(OIS::KC_A) || INPUT_MANAGER->isKeyDown(OIS::KC_LEFT))
	{
		if (mParentForward == ParentForward::NORTH)
			mParentGameObject->rotateLocal(45.0, Ogre::Vector3(0.0, 1.0, 0.0));
		if (mParentForward == ParentForward::EAST)
			mParentGameObject->rotateLocal(90.0, Ogre::Vector3(0.0, 1.0, 0.0));
		if (mParentForward == ParentForward::SOUTH)
			mParentGameObject->rotateLocal(45.0, Ogre::Vector3(0.0, -1.0, 0.0));
		mParentForward = ParentForward::WEST;

		mParentGameObject->translateWorld(Ogre::Vector3(-mParentSpeed, 0.0, 0.0));

		if (mMoving == false)
		{
			startingAction(false);
			mMoving = true;
		}
		animCounter += 1.0*dt;
	}
	else if (INPUT_MANAGER->isKeyDown(OIS::KC_S) || INPUT_MANAGER->isKeyDown(OIS::KC_DOWN))
	{
		if (mParentForward == ParentForward::NORTH)
			mParentGameObject->rotateLocal(90.0, Ogre::Vector3(0.0, 1.0, 0.0));
		if (mParentForward == ParentForward::WEST)
			mParentGameObject->rotateLocal(45.0, Ogre::Vector3(0.0, 1.0, 0.0));
		if (mParentForward == ParentForward::EAST)
			mParentGameObject->rotateLocal(45.0, Ogre::Vector3(0.0, -1.0, 0.0));
		mParentForward = ParentForward::SOUTH;

		mParentGameObject->translateWorld(Ogre::Vector3(0.0, -mParentSpeed, 0.0));

		if (mMoving == false)
		{
			startingAction(false);
			mMoving = true;
		}
		animCounter += 1.0*dt;
	}
	else if (INPUT_MANAGER->isKeyDown(OIS::KC_D) || INPUT_MANAGER->isKeyDown(OIS::KC_RIGHT))
	{
		if (mParentForward == ParentForward::NORTH)
			mParentGameObject->rotateLocal(45.0,Ogre::Vector3(0.0, -1.0, 0.0));
		if (mParentForward == ParentForward::WEST)
			mParentGameObject->rotateLocal(90.0, Ogre::Vector3(0.0, 1.0, 0.0));
		if (mParentForward == ParentForward::SOUTH)
			mParentGameObject->rotateLocal(45.0, Ogre::Vector3(0.0, 1.0, 0.0));

		mParentForward = ParentForward::EAST;
		mParentGameObject->translateWorld(Ogre::Vector3(mParentSpeed, 0.0, 0.0));

		if (mMoving == false)
		{
			startingAction(false);
			mMoving = true;
		}
		animCounter += 1.0*dt;
	}
	else
	{
		if (animCounter > 0.0 && mMoving)
		{
			mMoving = false;
			animCounter = 0.0f;
			endingAction();
		}
	}
	counter += 1.0*dt;
}



void ssurge::CameraComponent::startingAction(bool loop)
{
	ssurge::MeshComponent * mc = static_cast<ssurge::MeshComponent*>(GAME_OBJECT_MANAGER->findGameObject("Ninja")->getComponent(ssurge::ComponentType::MESH));
	mc->playAnim("Walk", true);
	mc->setMaterial(0, "Examples/Ninjatest");
	mc->setMaterial(1, "Examples/Ninjatest");
}



void ssurge::CameraComponent::endingAction()
{
	ssurge::MeshComponent * mc = static_cast<ssurge::MeshComponent*>(GAME_OBJECT_MANAGER->findGameObject("Ninja")->getComponent(ssurge::ComponentType::MESH));
	mc->playDefaultAnim();
	mc->setMaterial(0, "Examples/Ninja");
	mc->setMaterial(1, "Examples/Ninja");
}



void ssurge::CameraComponent::handleAction(std::string action, bool isStarting)
{
	std::cout << "Inside Camera Handle Action" << std::endl;
	if (action == "sprint")
	{
		if (isStarting)
			startingAction(false);
	}
}



void ssurge::CameraComponent::update(double dt)
{
	mParentGameObject = GAME_OBJECT_MANAGER->findGameObject("Ninja");///sets the parent game object

	Ogre::Vector3 parentPos = mParentGameObject->getSceneNode()->getPosition();
	Ogre::Quaternion q = mParentGameObject->getSceneNode()->getOrientation();
	mForward = q* parentPos;

	if (mCameraType == CameraType::FIRST_PERSON)
	{
		moveParentGameObject(dt);
		mCamera->setPosition(mOldPos);
		if(mSwitchMode)
		{
			mCamera->setOrientation(mOldOrientation);
			mSwitchMode = false;
		}

		ShowCursor(false);
		mCamera->yaw(Ogre::Radian(Ogre::Degree((float)(-mSens * INPUT_MANAGER->getAxis(1, true) * dt))));
		mCamera->pitch(Ogre::Radian(Ogre::Degree((float)(mSens * INPUT_MANAGER->getAxis(1, false) * dt))));
		if (mCamera->getOrientation().getPitch() >= Ogre::Radian(mYUp))///new radian variable goes here
		{
			Ogre::Radian newPitch = Ogre::Radian(mYUp);///and here
			newPitch -= mCamera->getOrientation().getPitch();
			mCamera->pitch(newPitch);
		}

		if (mCamera->getOrientation().getPitch() <= Ogre::Radian(-mYDown))///new radian variable goes here
		{
			Ogre::Radian newPitch = Ogre::Radian(-mYDown);///and here
			newPitch -= mCamera->getOrientation().getPitch();
			mCamera->pitch(newPitch);
		}
	}
	else if (mCameraType == CameraType::THIRD_PERSON)
	{
		if (mSwitchMode)
			mSwitchMode = false;
		ShowCursor(false);

		if (!mCamera->isAttached())
			mParentGameObject->addCamera(mCamera,false);

		Ogre::Vector3 absolutePos = mParentGameObject->getSceneNode()->_getDerivedPosition();
		Ogre::Vector3 cPos = mParentGameObject->getSceneNode()->getOrientation() * Ogre::Vector3(0.0, 0.0, -1.0);
		cPos += Ogre::Vector3(0.0, 20.0, 10.0);

		mCamera->setPosition(cPos);
		mCamera->lookAt(absolutePos);
		moveParentGameObject(dt);
	}
	else if (mCameraType == CameraType::FIXED)
	{
		if (mSwitchMode)
			mSwitchMode = false;
		mCamera->setPosition(mOwner->getSceneNode()->getPosition());
		mCamera->lookAt(mOwner->getSceneNode()->getPosition());
	}
	else
		ShowCursor(true);
}



void ssurge::CameraComponent::setupCamera(std::vector<ssurge::CameraType> modes, Ogre::Vector3 translate, Ogre::Vector3 angle, Ogre::Vector3 forward, Ogre::Vector3 up)
{
	mCameraType = modes.at(0);
	mTranslation = translate;
	mRotationAngle = angle;
	mForward = forward;
	mUp = up;
	mCameraModes = modes;

	//@ER: Don't actually call new -- use the createCamera method of the SceneManager.
	/*mCamera = new Ogre::Camera(mName, APPLICATION->getSceneManager());*/ // Not sure if this is the best place for it or not
	
	mCamera->setFOVy(Ogre::Radian(1.571f)); // An approximation of PI/2 or 90 deg
	mCamera->setOrientation(this->calculateOrientation(Ogre::Vector3(0.0,0.0,0.0)));
	mCamera->setPosition(mOwner->getSceneNode()->getPosition()); 
	mCamera->setDirection(forward);
	
	if(mCameraType != CameraType::FIRST_PERSON)
	{
		mCamera->setPosition(mCamera->getPosition() + translate); /// Initializes the Camera's position to that of the SceneNode plus our translation
		mCamera->setDirection(mCamera->getDirection() + translate); /// Adding the translation vector to the forward to make sure that the Camera still faces forward after the position shifting
	}
}

ssurge::CameraType ssurge::CameraComponent::nextMode() {
	for (int i = 0; i < mCameraModes.size(); i++)
	{
		if (mCameraModes.at(i) == mCameraType)
		{
			if (i == mCameraModes.size() - 1)
			{
				this->setMode(mCameraModes.at(0));
				//mCameraType = mCameraModes.at(0);
				if (mCameraType == CameraType::FIRST_PERSON)
					std::cout << "Camera Type: First Person." << std::endl;
				else if (mCameraType == CameraType::FIXED)
					std::cout << "Camera Type: Fixed." << std::endl;
				else
					std::cout << "Camera Type: Third Person." << std::endl;
				return mCameraType;
			}
			else
			{
				this->setMode(mCameraModes.at(i + 1));
				//mCameraType = mCameraModes.at(i + 1);
				if(mCameraType==CameraType::FIRST_PERSON)
					std::cout << "Camera Type: First Person." << std::endl;
				else if(mCameraType==CameraType::FIXED)
					std::cout << "Camera Type: Fixed." << std::endl;
				else
					std::cout << "Camera Type: Third Person." << std::endl;
				return mCameraType;
			}
		}
	}
}



void ssurge::CameraComponent::moveCamera(float x,float y,float z,double dt)
{
	if (mCameraType == CameraType::THIRD_PERSON)
		mCamera->move(Ogre::Vector3(x*mCamSpeed*dt, y*mCamSpeed*dt, z*mCamSpeed*dt));
	else if (mCameraType == CameraType::FIRST_PERSON)
		mCamera->move(Ogre::Vector3(x*mCamSpeed*dt, y*mCamSpeed*dt, z*mCamSpeed*dt));
	else
		std::cout << "cannot move camera. Probably in fixed mode." << std::endl;
}