#include <stdafx.h>
#include <InputManager.h>
#include <LogManager.h>
#include <Application.h>


// Template specialization to initialize the application's singleton pointer
template<>
ssurge::InputManager * ssurge::Singleton<ssurge::InputManager>::msSingleton = nullptr;



ssurge::InputManager::InputManager(Ogre::RenderWindow * win, std::string bindings_fname) : mKeyboard(NULL), mMouse(NULL), mOISInputManager(NULL),
            mPollDelay(2.0f), mPollTimer(0.0f)
{
	// Create the OIS Input Manager (very platform specific)
#ifdef _WIN32
	// Reference: www.ogre3d.org/tikiwiki/tiki-index.php?page=Using+OIS
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream temp_str;
	win->getCustomAttribute("WINDOW", &windowHnd);
	win->getCustomAttribute("WINDOW", &mHandle);
	temp_str << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), temp_str.str()));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
	mOISInputManager = OIS::InputManager::createInputSystem(pl);
#else
	// We should raise an exception if we get here -- the setup won't be properly run.
#endif

	// Create the other OIS input objects (this part should be cross-platform)
	mKeyboard = static_cast<OIS::Keyboard*>(mOISInputManager->createInputObject(OIS::OISKeyboard, true));
	if (mKeyboard)
		mKeyboard->setEventCallback(this);
	mMouse = static_cast<OIS::Mouse*>(mOISInputManager->createInputObject(OIS::OISMouse, true));
	if (mMouse)
	{
		mMouse->setEventCallback(this);

		// Get window size
		unsigned int width, height, depth;
		int left, top;
		win->getMetrics(width, height, depth, left, top);

		// Set mouse region
		this->setWindowExtents(width, height);
	}
		
	pollGamepads();

	// Load the device name maps
	loadDeviceNames();

	// Load the (initial) device bindings
	loadXMLBindings(bindings_fname);
}


ssurge::InputManager::~InputManager()
{
	// Clear out the bindings list
	std::map<unsigned int, Binding*>::iterator iter = mKeyBindings.begin();
	while (iter != mKeyBindings.end())
	{
		delete iter->second;
		iter->second = NULL;
		iter++;
	}
	mKeyBindings.clear();

	// ...and the gamepad button bindings
	iter = mGamepadButtonBindings.begin();
	while (iter != mGamepadButtonBindings.end())
	{
		delete iter->second;
		iter->second = NULL;
		iter++;
	}
	mGamepadButtonBindings.clear();

	// ...and the gamepad axis bindings
	iter = mGamepadAxisBindings.begin();
	while (iter != mGamepadAxisBindings.end())
	{
		delete iter->second;
		iter->second = NULL;
		iter++;
	}
	mGamepadAxisBindings.clear();
	
	for (CXBOXController * controller : mGamepads)
	{
		delete controller;
	}
	mGamepads.clear();
	

	// Destroy the keyboard
	if (mKeyboard && mOISInputManager)
		mOISInputManager->destroyInputObject(mKeyboard);
	mKeyboard = NULL;

	// Destroy the mouse
	if (mMouse && mOISInputManager)
		mOISInputManager->destroyInputObject(mMouse);
	mMouse = NULL;

	// Destroy the OIS Input Manager
	if (mOISInputManager)
		OIS::InputManager::destroyInputSystem(mOISInputManager);
	mOISInputManager = NULL;
}

void ssurge::InputManager::setWindowExtents(int width, int height) {
	// Set mouse region (if window resizes, we should alter this to reflect as well)
	const OIS::MouseState &mouseState = mMouse->getMouseState();
	mouseState.width = width;
	mouseState.height = height;
}



bool ssurge::InputManager::keyPressed(const OIS::KeyEvent & arg)
{
	unsigned int code = static_cast<unsigned int>(arg.key);

	std::map<unsigned int, Binding*>::iterator iter = mKeyBindings.find(code);
	if (iter != mKeyBindings.end())
	{
		if (iter->second->mAxis >= 0)
		{
			if (iter->second->mIsHoriz)
				mAxes[iter->second->mAxis].first += iter->second->mValue;
			else
				mAxes[iter->second->mAxis].second += iter->second->mValue;
		}
		else
			notifyListeners(iter->second->mName, true);
	}
	return true;
}



bool ssurge::InputManager::keyReleased(const OIS::KeyEvent & arg)
{
	unsigned int code = static_cast<unsigned int>(arg.key);

	std::map<unsigned int, Binding*>::iterator iter = mKeyBindings.find(code);
	if (iter != mKeyBindings.end())
	{
		if (iter->second->mAxis >= 0)
		{
			if (iter->second->mIsHoriz)
				mAxes[iter->second->mAxis].first -= iter->second->mValue;
			else
				mAxes[iter->second->mAxis].second -= iter->second->mValue;
		}
		else
			notifyListeners(iter->second->mName, false);
	}
	return true;
}



bool ssurge::InputManager::mouseMoved(const OIS::MouseEvent &arg)
{
	if (mMouseMode == MouseMode::FIXED) {
		OIS::MouseState &mutableMouseState = const_cast<OIS::MouseState &>(mMouse->getMouseState());

		int middleX = mMouse->getMouseState().width / 2;
		int middleY = mMouse->getMouseState().height / 2;

		float movedX = (mMouse->getMouseState().X.abs - middleX);
		movedX /= middleX;
		if (movedX > 1.0) movedX = 1.0;
		if (movedX < -1.0) movedX = -1.0;


		float movedY = (-mMouse->getMouseState().Y.abs + middleY);
		movedY /= middleY;
		if (movedY > 1.0) movedY = 1.0;
		if (movedY < -1.0) movedY = -1.0;


		mAxes[1].first = movedX;
		mAxes[1].second = movedY;
		mPrevMouseRelative.first = movedX;
		mPrevMouseRelative.second = movedY;

		mutableMouseState.X.abs = middleX;
		mutableMouseState.Y.abs = middleY;
		tagPOINT point;
		point.x = middleX;
		point.y = middleY;
		ClientToScreen(mHandle, &point);
		SetCursorPos(point.x, point.y);
	}
	if (mMouseMode == MouseMode::NORMAL) {
		mAxes[2].first = arg.state.X.abs;
		mAxes[2].second = arg.state.Y.abs;
	}
	return true;
}



bool ssurge::InputManager::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}



bool ssurge::InputManager::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

bool ssurge::InputManager::isKeyDown(const OIS::KeyCode k)
{
	return mKeyboard->isKeyDown(k);
}


bool ssurge::InputManager::isButtonDown(int button)
{
	return false;
}



void ssurge::InputManager::update(double dt)
{

	mPollTimer += dt;
	if (mPollTimer >= mPollDelay)
	{
		pollGamepads();
		mPollTimer = 0.0;
	}
	mMouseTimer += dt;
	if (mMouseTimer >= mMouseDelay)
	{
		if (mPrevMouseRelative.first == mAxes[1].first && mPrevMouseRelative.second == mAxes[1].second)
		{
			mAxes[1].first = 0.0;
			mAxes[1].second = 0.0;
		}
		mMouseTimer = 0.0;
	}

	mKeyboard->capture();
	mMouse->capture();

	// for each controller 0-3:
	for (int x = 0; x < 1; x++)
	{
		if (mGamepads[x]->IsConnected())
		{
			XINPUT_STATE curState = mGamepads[x]->GetState();
			if (curState.dwPacketNumber != mlastStates[x].dwPacketNumber) // if the pressed buttons updated
			{					
					if ((curState.Gamepad.wButtons & XINPUT_GAMEPAD_A) != (mlastStates[x].Gamepad.wButtons & XINPUT_GAMEPAD_A)) //if the state of the A button changed:
					{
						notifyListeners("jump", (curState.Gamepad.wButtons & XINPUT_GAMEPAD_A) ? true : false);
					}
					if ((curState.Gamepad.wButtons & XINPUT_GAMEPAD_B) != (mlastStates[x].Gamepad.wButtons & XINPUT_GAMEPAD_B))//if the state of the B button changed:
					{
						notifyListeners("dodge", (curState.Gamepad.wButtons & XINPUT_GAMEPAD_B) ? true : false);
					}
					if ((curState.Gamepad.wButtons & XINPUT_GAMEPAD_X) != (mlastStates[x].Gamepad.wButtons & XINPUT_GAMEPAD_X))//if the state of the X button changed:
					{
						notifyListeners("attack", (curState.Gamepad.wButtons & XINPUT_GAMEPAD_X) ? true : false);
					}
					if ((curState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != (mlastStates[x].Gamepad.wButtons & XINPUT_GAMEPAD_Y))//if the state of the Y button changed:
					{
						notifyListeners("sprint", (curState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) ? true : false);
					}
					if ((curState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != (mlastStates[x].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT))//if the state of the Left DPAD button changed:
					{
					}
					if ((curState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != (mlastStates[x].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT))//if the state of the Right DPAD button changed:
					{
					}
					if ((curState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != (mlastStates[x].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP))//if the state of the Up DPAD button changed:
					{
					}
					if ((curState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != (mlastStates[x].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN))//if the state of the Down DPAD button changed:
					{
					}
					if ((curState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != (mlastStates[x].Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER))//if the state of the Left Bumper button changed:
					{
					}
					if ((curState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != (mlastStates[x].Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER))//if the state of the Right Bumper button changed:
					{
					}
					if ((curState.Gamepad.wButtons & XINPUT_GAMEPAD_START) != (mlastStates[x].Gamepad.wButtons & XINPUT_GAMEPAD_START))//if the state of the Start button changed:
					{
					}
					if ((curState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != (mlastStates[x].Gamepad.wButtons & XINPUT_GAMEPAD_BACK))//if the state of the Back button changed:
					{
					}
					if ((curState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != (mlastStates[x].Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB))//if the state of the left thumbstick button changed:
					{
					}
					if ((curState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != (mlastStates[x].Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB))//if the state of the right thumbstick button changed:
					{
						bool starting = (curState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? true : false;
						if(starting)
							APPLICATION->getCameraComponent()->nextMode();

					}
					
					{

						/* Update the Left Thumbstick */

						float LX = curState.Gamepad.sThumbLX;
						float LY = curState.Gamepad.sThumbLY;

						float magnitude = sqrt(LX*LX + LY*LY);
						float normalizedLX = LX / magnitude;
						float normalizedLY = LY / magnitude;

						float normalizedMagnitude = 0;

						//check if the controller is outside a circular dead zone
						if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
						{
							//clip the magnitude at its expected maximum value
							if (magnitude > 32767) magnitude = 32767;

							//adjust magnitude relative to the end of the dead zone
							magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

							//optionally normalize the magnitude with respect to its expected range
							//giving a magnitude value of 0.0 to 1.0
							normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
						}
						else //if the controller is in the deadzone zero out the magnitude
						{
							magnitude = 0.0;
							normalizedMagnitude = 0.0;
						}
						float X = normalizedLX * normalizedMagnitude;
						float Y = normalizedLY * normalizedMagnitude;
						if (isnan(X)) X = 0.0; // Program will crash if these turn out to be NaN.  This usually happens only when the controller is disconnected and then reconnected.
						if (isnan(Y)) Y = 0.0; // Program will crash if these turn out to be NaN.  This usually happens only when the controller is disconnected and then reconnected.
						mAxes[0].first = X;
						mAxes[0].second = Y;
					}


					/* Update the Right Thumbstick */
					{
						float LX = curState.Gamepad.sThumbRX;
						float LY = curState.Gamepad.sThumbRY;

						float magnitude = sqrt(LX*LX + LY*LY);
						float normalizedLX = LX / magnitude;
						float normalizedLY = LY / magnitude;

						float normalizedMagnitude = 0;

						//check if the controller is outside a circular dead zone
						if (magnitude > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
						{
							//clip the magnitude at its expected maximum value
							if (magnitude > 32767) magnitude = 32767;

							//adjust magnitude relative to the end of the dead zone
							magnitude -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

							//optionally normalize the magnitude with respect to its expected range
							//giving a magnitude value of 0.0 to 1.0
							normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
						}
						else //if the controller is in the deadzone zero out the magnitude
						{
							magnitude = 0.0;
							normalizedMagnitude = 0.0;
						}
						float X = normalizedLX * normalizedMagnitude / 3;
						float Y = normalizedLY * normalizedMagnitude / 3;
						if (isnan(X)) X = 0.0; // Program will crash if these turn out to be NaN.  This usually happens only when the controller is disconnected and then reconnected.
						if (isnan(Y)) Y = 0.0; // Program will crash if these turn out to be NaN.  This usually happens only when the controller is disconnected and then reconnected.
						mAxes[1].first = X;
						mAxes[1].second = Y;
					}

					//update triggers
					{
						float leftTrigger = (float)curState.Gamepad.bLeftTrigger / 255;
						float rightTrigger = (float)curState.Gamepad.bRightTrigger / 255;

						mAxes[3].first = leftTrigger;
						mAxes[3].second = rightTrigger;
					}



					mlastStates.at(x) = curState; // Update the previous state of the controller so that next frame we can tell if the controller's state changed.
			}

		}
	}
}


float ssurge::InputManager::getAxis(unsigned int axis, bool isHoriz)
{
	if (axis < mAxes.size())
	{
		if (isHoriz)
			return mAxes[axis].first;
		else
			return mAxes[axis].second;
	}
	return 0.0f;
}


void ssurge::InputManager::loadDeviceNames()
{
	std::vector<std::string> names;
	std::vector<unsigned int> values;

	// Note: These are from OISKeyboard.h.  I used some nifty regular expressions to write this code:-)
	names.push_back("KC_UNASSIGNED"); values.push_back(0x00);		names.push_back("KC_ESCAPE"); values.push_back(0x01);
	names.push_back("KC_1"); values.push_back(0x02);				names.push_back("KC_2"); values.push_back(0x03);
	names.push_back("KC_3"); values.push_back(0x04);				names.push_back("KC_4"); values.push_back(0x05);
	names.push_back("KC_5"); values.push_back(0x06);				names.push_back("KC_6"); values.push_back(0x07);
	names.push_back("KC_7"); values.push_back(0x08);				names.push_back("KC_8"); values.push_back(0x09);
	names.push_back("KC_9"); values.push_back(0x0A);				names.push_back("KC_0"); values.push_back(0x0B);
	names.push_back("KC_MINUS"); values.push_back(0x0C);			names.push_back("KC_EQUALS"); values.push_back(0x0D);
	names.push_back("KC_BACK"); values.push_back(0x0E);				names.push_back("KC_TAB"); values.push_back(0x0F);
	names.push_back("KC_Q"); values.push_back(0x10);				names.push_back("KC_W"); values.push_back(0x11);
	names.push_back("KC_E"); values.push_back(0x12);				names.push_back("KC_R"); values.push_back(0x13);
	names.push_back("KC_T"); values.push_back(0x14);				names.push_back("KC_Y"); values.push_back(0x15);
	names.push_back("KC_U"); values.push_back(0x16);				names.push_back("KC_I"); values.push_back(0x17);
	names.push_back("KC_O"); values.push_back(0x18);				names.push_back("KC_P"); values.push_back(0x19);
	names.push_back("KC_LBRACKET"); values.push_back(0x1A);			names.push_back("KC_RBRACKET"); values.push_back(0x1B);
	names.push_back("KC_RETURN"); values.push_back(0x1C);			names.push_back(	"KC_LCONTROL"); values.push_back(0x1D);
	names.push_back("KC_A"); values.push_back(0x1E);				names.push_back("KC_S"); values.push_back(0x1F);
	names.push_back("KC_D"); values.push_back(0x20);				names.push_back("KC_F"); values.push_back(0x21);
	names.push_back("KC_G"); values.push_back(0x22);				names.push_back("KC_H"); values.push_back(0x23);
	names.push_back("KC_J"); values.push_back(0x24);				names.push_back("KC_K"); values.push_back(0x25);
	names.push_back("KC_L"); values.push_back(0x26);				names.push_back("KC_SEMICOLON"); values.push_back(0x27);
	names.push_back("KC_APOSTROPHE"); values.push_back(0x28);		names.push_back("KC_GRAVE"); values.push_back(0x29);
	names.push_back("KC_LSHIFT"); values.push_back(0x2A);			names.push_back("KC_BACKSLASH"); values.push_back(0x2B);
	names.push_back("KC_Z"); values.push_back(0x2C);				names.push_back("KC_X"); values.push_back(0x2D);
	names.push_back("KC_C"); values.push_back(0x2E);				names.push_back("KC_V"); values.push_back(0x2F);
	names.push_back("KC_B"); values.push_back(0x30);				names.push_back("KC_N"); values.push_back(0x31);
	names.push_back("KC_M"); values.push_back(0x32);				names.push_back("KC_COMMA"); values.push_back(0x33);
	names.push_back("KC_PERIOD"); values.push_back(0x34);			names.push_back("KC_SLASH"); values.push_back(0x35);
	names.push_back("KC_RSHIFT"); values.push_back(0x36);			names.push_back("KC_MULTIPLY"); values.push_back(0x37);
	names.push_back("KC_LMENU"); values.push_back(0x38);			names.push_back("KC_SPACE"); values.push_back(0x39);
	names.push_back("KC_CAPITAL"); values.push_back(0x3A);			names.push_back("KC_F1"); values.push_back(0x3B);
	names.push_back("KC_F2"); values.push_back(0x3C);				names.push_back("KC_F3"); values.push_back(0x3D);
	names.push_back("KC_F4"); values.push_back(0x3E);				names.push_back("KC_F5"); values.push_back(0x3F);
	names.push_back("KC_F6"); values.push_back(0x40);				names.push_back("KC_F7"); values.push_back(0x41);
	names.push_back("KC_F8"); values.push_back(0x42);				names.push_back("KC_F9"); values.push_back(0x43);
	names.push_back("KC_F10"); values.push_back(0x44);				names.push_back("KC_NUMLOCK"); values.push_back(0x45);
	names.push_back("KC_SCROLL"); values.push_back(0x46);			names.push_back("KC_NUMPAD7"); values.push_back(0x47);
	names.push_back("KC_NUMPAD8"); values.push_back(0x48);			names.push_back("KC_NUMPAD9"); values.push_back(0x49);
	names.push_back("KC_SUBTRACT"); values.push_back(0x4A);			names.push_back("KC_NUMPAD4"); values.push_back(0x4B);
	names.push_back("KC_NUMPAD5"); values.push_back(0x4C);			names.push_back("KC_NUMPAD6"); values.push_back(0x4D);
	names.push_back("KC_ADD"); values.push_back(0x4E);				names.push_back("KC_NUMPAD1"); values.push_back(0x4F);
	names.push_back("KC_NUMPAD2"); values.push_back(0x50);			names.push_back("KC_NUMPAD3"); values.push_back(0x51);
	names.push_back("KC_NUMPAD0"); values.push_back(0x52);			names.push_back("KC_DECIMAL"); values.push_back(0x53);
	names.push_back("KC_OEM_102"); values.push_back(0x56);			names.push_back("KC_F11"); values.push_back(0x57);
	names.push_back("KC_F12"); values.push_back(0x58);				names.push_back("KC_F13"); values.push_back(0x64);
	names.push_back("KC_F14"); values.push_back(0x65);				names.push_back("KC_F15"); values.push_back(0x66);
	names.push_back("KC_KANA"); values.push_back(0x70);				names.push_back("KC_ABNT_C1"); values.push_back(0x73);
	names.push_back("KC_CONVERT"); values.push_back(0x79);			names.push_back("KC_NOCONVERT"); values.push_back(0x7B);
	names.push_back("KC_YEN"); values.push_back(0x7D);				names.push_back("KC_ABNT_C2"); values.push_back(0x7E);
	names.push_back("KC_NUMPADEQUALS"); values.push_back(0x8D);		names.push_back("KC_PREVTRACK"); values.push_back(0x90);
	names.push_back("KC_AT"); values.push_back(0x91);				names.push_back("KC_COLON"); values.push_back(0x92);
	names.push_back("KC_UNDERLINE"); values.push_back(0x93);		names.push_back("KC_KANJI"); values.push_back(0x94);
	names.push_back("KC_STOP"); values.push_back(0x95);				names.push_back("KC_AX"); values.push_back(0x96);
	names.push_back("KC_UNLABELED"); values.push_back(0x97);		names.push_back("KC_NEXTTRACK"); values.push_back(0x99);
	names.push_back("KC_NUMPADENTER"); values.push_back(0x9C);		names.push_back("KC_RCONTROL"); values.push_back(0x9D);
	names.push_back("KC_MUTE"); values.push_back(0xA0);				names.push_back("KC_CALCULATOR"); values.push_back(0xA1);
	names.push_back("KC_PLAYPAUSE"); values.push_back(0xA2);		names.push_back("KC_MEDIASTOP"); values.push_back(0xA4);
	names.push_back("KC_VOLUMEDOWN"); values.push_back(0xAE);		names.push_back("KC_VOLUMEUP"); values.push_back(0xB0);
	names.push_back("KC_WEBHOME"); values.push_back(0xB2);			names.push_back("KC_NUMPADCOMMA"); values.push_back(0xB3);
	names.push_back("KC_DIVIDE"); values.push_back(0xB5);			names.push_back("KC_SYSRQ"); values.push_back(0xB7);
	names.push_back("KC_RMENU"); values.push_back(0xB8);			names.push_back("KC_PAUSE"); values.push_back(0xC5);
	names.push_back("KC_HOME"); values.push_back(0xC7);				names.push_back("KC_UP"); values.push_back(0xC8);
	names.push_back("KC_PGUP"); values.push_back(0xC9);				names.push_back("KC_LEFT"); values.push_back(0xCB);
	names.push_back("KC_RIGHT"); values.push_back(0xCD);			names.push_back("KC_END"); values.push_back(0xCF);
	names.push_back("KC_DOWN"); values.push_back(0xD0);				names.push_back("KC_PGDOWN"); values.push_back(0xD1);
	names.push_back("KC_INSERT"); values.push_back(0xD2);			names.push_back("KC_DELETE"); values.push_back(0xD3);
	names.push_back("KC_LWIN"); values.push_back(0xDB);				names.push_back("KC_RWIN"); values.push_back(0xDC);
	names.push_back("KC_APPS"); values.push_back(0xDD);				names.push_back("KC_POWER"); values.push_back(0xDE);
	names.push_back("KC_SLEEP"); values.push_back(0xDF);			names.push_back("KC_WAKE"); values.push_back(0xE3);
	names.push_back("KC_WEBSEARCH"); values.push_back(0xE5);		names.push_back("KC_WEBFAVORITES"); values.push_back(0xE6);
	names.push_back("KC_WEBREFRESH"); values.push_back(0xE7);		names.push_back("KC_WEBSTOP"); values.push_back(0xE8);
	names.push_back("KC_WEBFORWARD"); values.push_back(0xE9);		names.push_back("KC_WEBBACK"); values.push_back(0xEA);
	names.push_back("KC_MYCOMPUTER"); values.push_back(0xEB);		names.push_back("KC_MAIL"); values.push_back(0xEC);
	names.push_back("KC_MEDIASELECT"); values.push_back(0xED);

	for (unsigned int i = 0; i < names.size(); i++)
	{
		mKeyNameToID[names[i]] = values[i];
		mKeyIDToName[values[i]] = names[i];
	}
}


void ssurge::InputManager::notifyListeners(std::string action, bool isStarting)
{
	for (std::list<GameObject*>::iterator i = mListeners.begin(); i != mListeners.end(); i++)
	{
		(*i)->handleAction(action, isStarting);
	}
} 


void ssurge::InputManager::pollGamepads()
{
	//Xinput allows for up to four controllers
	
	if (mGamepads.size() < 4)
	{
		//For Player 1-4, create a gamepad for them, even if no controllers are connected.
		for (int x = 1; x < 5; x++) {
			CXBOXController * newController = new CXBOXController(x);
			mGamepads.push_back(newController);
		}
	}

	if (mlastStates.size() < 4)
	{
		//For Player 1-4, create a gamepad for them, even if no controllers are connected.
		for (int x = 1; x < 5; x++) {
			
			mlastStates.push_back(mGamepads.at(x - 1)->GetState());
		}
	}
}


void ssurge::InputManager::loadXMLBindings(std::string fname)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(fname.c_str());
	if (result && doc.child("device_map"))
	{
		// Parse keyboard bindings
		pugi::xml_node group = doc.child("device_map").child("key_bindings");
		pugi::xml_node entry;
		pugi::xml_attribute attrib, attrib2, attrib3, attrib4;
		if (!group.empty())
		{
			entry = group.first_child();
			while (!entry.empty())
			{
				// See if this is an action-type entry
				attrib = entry.attribute("action");
				attrib2 = entry.attribute("key");
				if (!attrib.empty() && !attrib2.empty())
				{
					std::string key_name = attrib2.as_string();
					std::map<std::string, unsigned int>::iterator iter = mKeyNameToID.find(key_name);
					if (iter != mKeyNameToID.end())
					{
						unsigned int code = iter->second;
						mKeyBindings[code] = new Binding(attrib.as_string(), code);
					}
				}

				// See if this is an axis-type entry (technically they could be both an action and an axis)...
				attrib = entry.attribute("isHoriz");
				attrib2 = entry.attribute("axis_number");
				attrib3 = entry.attribute("key");
				attrib4 = entry.attribute("axis_value");
				if (!attrib.empty() && !attrib2.empty() && !attrib3.empty() && !attrib4.empty())
				{
					int axis = attrib2.as_int();
					std::map<std::string, unsigned int>::iterator iter = mKeyNameToID.find(attrib3.as_string());
					if (iter != mKeyNameToID.end() && axis >= 0)
					{
						// See if we need to add a new axis
						while (mAxes.size() < (unsigned int)axis + 1)
							mAxes.push_back(std::pair<float, float>(0.0f, 0.0f));
						unsigned int code = iter->second;
						mKeyBindings[code] = new Binding(axis, code, attrib4.as_float(), attrib.as_bool());
					}
				}

				// Get the next binding entry
				entry = entry.next_sibling();
			}
		}

		// Parse gamepad bindings
		group = doc.child("device_map").child("gamepad_bindings");
		if (!group.empty())
		{
			entry = group.first_child();
			while (!entry.empty())
			{
				// See if this is a button entry
				attrib = entry.attribute("action");
				attrib2 = entry.attribute("button");
				if (attrib && attrib2)
				{
					unsigned int code = attrib2.as_int();
					mGamepadButtonBindings[code] = new Binding(attrib.as_string(), code);
				}

				// See if this is a dpad entry
				attrib2 = entry.attribute("mask");
				if (attrib && attrib2)
				{
					unsigned int mask = attrib2.as_int();
					mGamepadButtonBindings[mask] = new Binding(attrib.as_string(), mask);
				}

				// See if this is an axis entry.
				attrib = entry.attribute("isHoriz");
				attrib2 = entry.attribute("axis_number");
				attrib3 = entry.attribute("device_axis");
				attrib4 = entry.attribute("device_max");
				if (!attrib.empty() && !attrib2.empty() && !attrib3.empty() && !attrib4.empty())
				{
					bool is_horiz = attrib.as_bool();
					unsigned int axis = attrib2.as_int();
					unsigned int daxis = attrib3.as_int();
					int max_val = attrib4.as_int();
					while (axis >= mAxes.size())
						mAxes.push_back(std::pair<float, float>(0.0f, 0.0f));
					mGamepadAxisBindings[daxis] = new Binding(axis, daxis, (float)max_val, is_horiz);
				}

				attrib = entry.attribute("axis_number");
				attrib2 = entry.attribute("trigger");
				if (!attrib.empty() && !attrib2.empty())
				{
					unsigned int axis = attrib.as_int();
					while (axis >= mAxes.size())
						mAxes.push_back(std::pair<float, float>(0.0f, 0.0f));
				}

				// Get the next binding entry.
				entry = entry.next_sibling();
			}
		}
		
		group = doc.child("device_map").child("mouse_bindings");
		if (!group.empty())
		{
			entry = group.first_child();
			while (!entry.empty())
			{
				// See if this is a mouse entry
				attrib = entry.attribute("axis_number");
				if (!attrib.empty())
				{
					unsigned int axis = attrib.as_int();
					while (axis >= mAxes.size())
						mAxes.push_back(std::pair<float, float>(0.0f, 0.0f));
				}

				// Get the next binding entry.
				entry = entry.next_sibling();
			}
		}
		
	}
	else
		LOG_MANAGER->log("Failed to load input device bindings file!", LL_ERROR);
}



void ssurge::InputManager::addListener(ssurge::GameObject * g)
{
	bool found = false;
	std::list<GameObject*>::iterator iter = mListeners.begin();
	while (iter != mListeners.end())
	{
		if ((*iter) == g)
		{
			found = true;
			break;
		}
	}
	if (!found)
		mListeners.push_back(g);
}



void ssurge::InputManager::removeListener(ssurge::GameObject * g)
{
	std::list<GameObject*>::iterator iter = mListeners.begin();
	while (iter != mListeners.end())
	{
		if ((*iter) == g)
			iter = mListeners.erase(iter);
		else
			iter++;
	}
}

ssurge::InputManager::CXBOXController * ssurge::InputManager::getController(int p)
{
	return mGamepads.at(p);
}




//XInput Controller class methods
ssurge::InputManager::CXBOXController::CXBOXController(int playerNumber)
{
	// Set the Controller Number
	_controllerNum = playerNumber - 1;
}

XINPUT_STATE ssurge::InputManager::CXBOXController::GetState()
{
	// Zeroise the state
	ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

	// Get the state
	XInputGetState(_controllerNum, &_controllerState);

	return _controllerState;
}

bool ssurge::InputManager::CXBOXController::IsConnected()
{
	// Zeroise the state
	ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

	// Get the state
	DWORD Result = XInputGetState(_controllerNum, &_controllerState);

	if (Result == ERROR_SUCCESS)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ssurge::InputManager::CXBOXController::Vibrate(int leftVal, int rightVal)
{
	// Create a Vibraton State
	XINPUT_VIBRATION Vibration;

	// Zeroise the Vibration
	ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));

	// Set the Vibration Values
	Vibration.wLeftMotorSpeed = leftVal;
	Vibration.wRightMotorSpeed = rightVal;

	// Vibrate the controller
	XInputSetState(_controllerNum, &Vibration);
}