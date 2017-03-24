#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

#include <stdafx.h>
#include <Singleton.h>
#include <GameObject.h>

// We need the Windows Header and the XInput Header for controllers
#include <windows.h>
#include <windef.h>
#include <XInput.h>

// Now, the XInput Library
// NOTE: COMMENT THIS OUT IF YOU ARE NOT USING
// A COMPILER THAT SUPPORTS THIS METHOD OF LINKING LIBRARIES
#pragma comment(lib, "XInput9_1_0.lib")

/// Convenience macro to make accessing the singleton a bit less cumbersome
#define INPUT_MANAGER ssurge::InputManager::getSingletonPtr()




//XINPUT_GAMEPAD_DPAD_UP          0x00000001
//XINPUT_GAMEPAD_DPAD_DOWN        0x00000002
//XINPUT_GAMEPAD_DPAD_LEFT        0x00000004
//XINPUT_GAMEPAD_DPAD_RIGHT       0x00000008
//XINPUT_GAMEPAD_START            0x00000010
//XINPUT_GAMEPAD_BACK             0x00000020
//XINPUT_GAMEPAD_LEFT_THUMB       0x00000040
//XINPUT_GAMEPAD_RIGHT_THUMB      0x00000080
//XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100
//XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200
//XINPUT_GAMEPAD_A                0x1000
//XINPUT_GAMEPAD_B                0x2000
//XINPUT_GAMEPAD_X                0x4000
//XINPUT_GAMEPAD_Y                0x8000

namespace ssurge
{
	/// The InputManager is, unsurprisingly, responsible for all things
	/// input-related.  It contains an Observer system to allow interested
	/// GameObjects to be notified when an input event happens.  It also
	/// supports "device-polling" (querying whether a button / key / etc is pressed)
	class InputManager : public Singleton<InputManager>, public OIS::KeyListener, public OIS::MouseListener
	{
	/***** NESTED STRUCTURES *****/
	protected:
		/// This simple structure is responsible for holding a binding between
		/// a keyboard / mouse / gamepad device (e.g. button / key) and a descriptive
		/// action name.
		class Binding
		{
		public:
			std::string mName;				// Only used for actions
			int mAxis;						// -1 for actions, 0, 1, ... for axes.
			int mDeviceCode;				// OIS::KeyCode (as int) for button, a dpad code or a gamepad range-max value
			float mValue;					// Only used for axes.
			bool mIsHoriz;					// Only used for axes.

			/// This constructor is used to create an action (button) binding
			Binding(std::string name, int code) : mName(name), mAxis(-1), mDeviceCode(code), mValue(0.0f), mIsHoriz(false) {}
			
			/// This constructor is used to create an axis binding.
			Binding(unsigned int axis, int code, float val, bool horiz) : mName(""), mAxis(axis), mDeviceCode(code), mValue(val), mIsHoriz(horiz) {}
		};

		class CXBOXController
		{
		private:
			/// Contains the state of the controller
			XINPUT_STATE _controllerState;
			/// Contains which controller this is out of the connected controllers (0-3)
			int _controllerNum;
		public:
			/// Constructor
			CXBOXController(int playerNumber);
			/// Updates and returns the state of the controller so that checks can be made on input
			XINPUT_STATE GetState();
			/// Checks to ensure that the controller is connected, and will return ERROR_SUCCESS if successful
			bool IsConnected();
			/// Call this to vibrate the controller
			void Vibrate(int leftVal = 0, int rightVal = 0);
		};

		/// Each mode allows the mouse to behave differently
		enum class MouseMode
		{
			UNKNOWN,
			NORMAL,
			FIXED
		};

	/***** ATTRIBUTES *****/
	protected:

		/// A list of all of the controllers' states from the previous update frame
		std::vector<XINPUT_STATE> mlastStates;

		/// The OIS Input Manager
		OIS::InputManager * mOISInputManager;

		/// The OIS Keyboard object
		OIS::Keyboard * mKeyboard;

		/// The OIS Mouse object
		OIS::Mouse * mMouse;

		/// The current mode of the mouse
		InputManager::MouseMode mMouseMode = MouseMode::FIXED;

		/// The OIS Gamepad object(s)
		std::vector<CXBOXController*> mGamepads;

		/// The list of listeners
		std::list<GameObject*> mListeners;

		/// The amount of time since we've done the last joystick poll (in seconds)
		double mPollTimer;

		/// The amount of time we wait between joystick polls (in seconds)
		double mPollDelay;

		///The amount of time we wait between zeroing out the relative mouse coordinates
		double mMouseDelay = 0.1;

		///The amount of time since we've done the last relative mouse coordinate zeroing
		double mMouseTimer = 0.0;

		/// A mapping from OIS names (of the form "KC_??") to key enum values (as int)
		std::map<std::string, unsigned int> mKeyNameToID;

		/// A mapping from OIS enum key values (as int) to OIS key names (of the form "KC_??").
		std::map<unsigned int, std::string> mKeyIDToName;

		/// A map (by OIS::KeyCode (as an int)) action / axis bindings
		std::map<unsigned int, Binding*> mKeyBindings;

		/// A map (by button number) action bindings
		std::map<unsigned int, Binding*> mGamepadButtonBindings;

		/// A map (by device axis) gamepad axis bindings
		std::map<unsigned int, Binding*> mGamepadAxisBindings;

		/// The axes we're tracking
		std::vector<std::pair<float, float>> mAxes;

		/// The previous relative offset for the mouse
		std::pair<float, float> mPrevMouseRelative;

		/// The window handle for the ogre window.
		HWND mHandle;


	/***** CONSTRUCTORS / DESTRUCTORS *****/
	public:
		/// The Basic constructor.  
		InputManager(Ogre::RenderWindow * win, std::string bindings_fname);

		/// Destructor
		virtual ~InputManager();


	/***** CALLBACK METHODS (not meant to be called directly -- called by "someone" else [OIS input objects in this case]) *****/
	protected:
		/// Called when a key is pressed
		virtual bool keyPressed(const OIS::KeyEvent & arg);

		/// Called when a key is released
		virtual bool keyReleased(const OIS::KeyEvent & arg);

		/// Called when the mouse moves
		virtual bool mouseMoved(const OIS::MouseEvent &arg);

		/// Called when the mouse button is pressed
		virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

		/// Called when the mouse button is released
		virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
			


	/***** METHODS *****/
	public:
		/// This method should be called once per frame
		virtual void update(double dt);

		/// Set the amount of time we wait between polls for added / removed devices
		void setPollDelay(float d) { mPollDelay = d; }

		/// Gets the value of an axis (if this axis isn't defined, 0.0f is returned)
		float getAxis(unsigned int axis_num, bool isHoriz);

		/// Loads an xml bindings file.  For now, this has to include path information...
		void loadXMLBindings(std::string fname);

		/// Registers the given game object as a listener
		void addListener(GameObject * g);

		/// Removes the given listener from our list of listeners (it is not an error to remove a non-existent listener)
		void removeListener(GameObject * g);

		/// Returns the controller for the specified player
		CXBOXController * getController(int p);

		/// Lets the mouse know the extents of the window, should only have to call this when the window size changes
		void setWindowExtents(int width, int height);

		///



	protected:

		/// Polls for joystick adds / removals
		virtual void pollGamepads();

		/// Load device names
		void loadDeviceNames();

		/// Notifies all listeners that this event just occurred
		virtual void notifyListeners(std::string action, bool isStarting);

		/// Returns the (string) action name for this key-press code ("" if no binding found)
		std::string findKeyAction(unsigned int keyCode);

		/// Returns true if the given key code is an axis binding.  If it is, the name and value parameters will be
		/// modified (they are left unchanged if not)
		bool findKeyAxis(const unsigned int keyCode, std::string & name, float & value);

	public:
		/// Returns true if the given key is currently down
		bool isKeyDown(const OIS::KeyCode k);

		/// Returns true if this button is pressed on *any* gamepad
		bool isButtonDown(int button);
	};





}

#endif