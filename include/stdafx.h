// Standard C++ libraries
#include <sstream>
#include <fstream>
#include <ctime>
#include <iostream>
#include <string>
#include <map>
#include <utility>

#include <Windows.h>

// External libraries (in the dependencies folder)
#include <Ogre.h>
#include <Overlay\OgreOverlay.h>
#include <Overlay\OgreOverlayManager.h>
#include <Overlay\OgreOverlaySystem.h>
#include <Overlay\OgreOverlayContainer.h>
#include <Overlay\OgreFontManager.h>
#include <Overlay\OgreTextAreaOverlayElement.h>
#include <OgreCompositorLogic.h>


#include <OIS.h>

#include <python.h>
#include <frameobject.h>		// For some error-handling in python
#include <structmember.h>		// For some class members in python
#include <cstdarg>				// For variadic constructor argument handling of c++ python tuple wrapper

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}

#include <pugixml.hpp>

#include <irrKlang.h>

#include <SFML/Network.hpp>
#include <SFML/System.hpp>

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>