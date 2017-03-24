#ifndef _LUA_SCRIPT_MANAGER_H_
#define _LUA_SCRIPT_MANAGER_H_

#include <stdafx.h>
#include <Singleton.h>

/// Convenience macro to make accessing the singleton a bit less cumbersome
#define LUA_SCRIPT_MANAGER ssurge::LuaScriptManager::getSingletonPtr()

namespace ssurge
{
	/// The Lua Script Manager is responsible for managing the embedded lua interpreter.
	class LuaScriptManager : public Singleton<LuaScriptManager>
	{
		/***** ATTRIBUTES *****/
	protected:
		lua_State * mState;

		/***** CONSTRUCTORS / DESTRUCTORS *****/
	public:
		/// One and only (singleton) constructor
		LuaScriptManager();

		/// Destructor
		virtual ~LuaScriptManager();

		/***** METHODS *****/
	public:

		/// Loads a script from disk into memory.  Fname must include path (for now).  Returns 0 on success
		int loadScript(std::string fname);

		/// Retrieves a formatted error string showing any information obtainable if something goes wrong in Python.
		std::string getErrorString();
	};

}


#endif