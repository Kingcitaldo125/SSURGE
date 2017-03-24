#include <stdafx.h>
#include <LuaScriptTopLevel.h>
#include <LogManager.h>


int ssurge::lua_script::log(lua_State *L)
{
	std::string msg = luaL_checkstring(L, 1);

	LOG_MANAGER->log(msg, LL_SCRIPT);

	/*size_t nbytes = sizeof(ssurge::script::lua::LuaGameObject);
	LuaGameObject * lgo = (LuaGameObject *)lua_newuserdata(L, nbytes);*/
	return 1;
}

int ssurge::lua_script::setBackgroundColor(lua_State *L)
{
	return 1;
}

int ssurge::lua_script::getAxis(lua_State *L)
{
	return 1;
}

int ssurge::lua_script::getActionDown(lua_State *L)
{
	return 1;
}