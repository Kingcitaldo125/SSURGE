#include <stdafx.h>
#include <LScriptManager.h>
#include <LScriptMain.h>
#include <LogManager.h>

// Template specialization to initialize singleton attribute
template<>
ssurge::LuaScriptManager * ssurge::Singleton<ssurge::LuaScriptManager>::msSingleton = nullptr;

ssurge::LuaScriptManager::LuaScriptManager()
{
	mState = luaL_newstate(); // load the interpreter
	luaL_openlibs(mState); // load default libraries
	ssurge::lua_script::luaopen_ssurge(mState); // loads the Gameobject library
	luaL_dofile(mState, "test.lua");
}
ssurge::LuaScriptManager::~LuaScriptManager()
{
	if (mState)
		lua_close(mState);
}