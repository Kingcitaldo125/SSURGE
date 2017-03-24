#ifndef _CAMERA_COMPONENT_H_
#define _CAMERA_COMPONENT_H_

#include <stdafx.h>
#include <Component.h>

namespace ssurge
{

	enum class CameraType
	{
		FIXED,
		FIRST_PERSON,
		THIRD_PERSON
	};

	enum class ParentForward
	{
		WEST,
		SOUTH,
		NORTH,
		EAST
	};

	// A forward-reference to the GameObject class (put here to avoid a circular
	// dependency)
	class GameObject;

	/// CameraComponents are responsible for  
	
	class CameraComponent : public Component
	{
		/***** ATTRIBUTES *****/
	protected:
		/// The Ogre Camera instance (a camera object)
		Ogre::Camera * mCamera;

	private:
		/// A 'camera type' to denote a first or third person Camera
		CameraType mCameraType;

		/// The name to create the Camera. Name is the mOwner->getName() + "Camera"
		std::string mName;

		/// The rotation angles to add to the mOwner's Orientation, to make sure the Camera has the proper Orientation
		Ogre::Vector3 mRotationAngle;

		/// The translation vector for the positioning of the Camera
		Ogre::Vector3 mTranslation;

		/// The forward vector of the object the camera is attached, to point the camera in the proper direction
		Ogre::Vector3 mForward;

		/// The up vector of the object the camera is attached, not yet in use
		Ogre::Vector3 mUp;

		/// The Sensitivity of the camera
		float mSens;

		///The y up and y down clipping for the camera
		float mYUp, mYDown;

		///camera movement speed(s)
		float mCamSpeed;

		///camera rotation speed
		float mCamRotSpeed;

		///parent game object speed
		float mParentSpeed;

		///List of usable CameraTypes for this component only
		std::vector<ssurge::CameraType> mCameraModes;

		/// The parent game object
		GameObject* mParentGameObject;

		/// Tells us if we added the GameObject as a listener
		bool mAdded;

		///Is the parent game object moving???
		bool mMoving;

		///Where the parent is facing
		ParentForward mParentForward;

		///Old first person cam pos
		Ogre::Vector3 mOldPos;

		///Old first person camera orientation
		Ogre::Quaternion mOldOrientation;

		bool mSwitchMode;

		/***** CONSTRUCTORS / DESTRUCTORS *****/
	public:
		/// Default constructor
		CameraComponent(GameObject * owner, std::vector<ssurge::CameraType> modes);

		/// Destructor
		virtual ~CameraComponent();

		/***** METHODS *****/
	public:
		/// Gets the type of this component
		ComponentType getType() override { return ComponentType::CAMERA; }

		//@ER: Do we need this?
		/// A simple way to get the pointer to the Camera, since this class isn't fully developed yet
		Ogre::Camera* getCamera() { return mCamera; }

		//@ER: Do we need this?
		/// If a camera is already defined, then it sets the component's camera to one passed to this;
		void setCamera(Ogre::Camera * c) { mCamera = c; }

		/// An easy going method to calculate the Orientation of for the camera
		Ogre::Quaternion calculateOrientation(Ogre::Vector3 rotAngle);

		/// Updates this component
		virtual void update(double dt) override;

		/// Returns the camera's sensitivity
		float getSens() { return mSens; }

		/// Sets the camera's sensitivity
		void setSens(float sens) { mSens = sens; }

		//@ER: I see this as the main way we'll set up the internal camera.  We may need to move the axes to Kierra's code.
		/// Sets up the Camera
		void setupCamera(std::vector<ssurge::CameraType> modes, Ogre::Vector3 translate, Ogre::Vector3 angle, Ogre::Vector3 forward, Ogre::Vector3 up);

		//@PA: For later use. Will rotate the camera when the mouse is moved.(We took this out)
		//void rotateCamera(float rotSpeed);

		//@PA: we may/may not need this but a way to change between first/third person.
		void setMode(CameraType bl) { mCameraType = bl; }

		ssurge::CameraType nextMode();

		///Sets the up pitch limit
		void setYUp(float y) { mYUp = y; }

		///Sets the down pitch limit
		void setYDown(float y) { mYDown = y; }

		///move/translate the camera...Depending on the camera mode.
		void moveCamera(float x, float y, float z, double dt);

		///Gets the current parent game object
		GameObject* getParentGameObject() { return mParentGameObject; }

		///Sets the parent game object. When the camera is created,
		///it is not bound to any particular game object
		//void setParentGameObject(std::string name) { mParentGameObject = GAME_OBJECT_MANAGER->findGameObject(name); }

		/// Handles movement, with certain key presses
		void handleCameraMovement(double dt);

		void handleCameraRotation(std::string axis);

		///Moves the parent game object
		void moveParentGameObject(double dt);

		/// Basic handleaction like the one in SimplePlayer
		void handleAction(std::string action, bool isStarting);

		///
		void ssurge::CameraComponent::startingAction(bool loop);

		void ssurge::CameraComponent::endingAction();
	};
}

#endif
