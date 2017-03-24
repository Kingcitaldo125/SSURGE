#include <stdafx.h>
#include <LScriptMain.h>


int ssurge::lua_script::luaopen_ssurge(lua_State *L) 
{
	int lib_id, meta_id;

	// newclass = {} 
	lua_createtable(L, 0, 0);
	lib_id = lua_gettop(L);

	// metatable = {} 
	luaL_newmetatable(L, "ssurge");
	meta_id = lua_gettop(L);
	luaL_setfuncs(L, _meta, 0);

	// metatable.__index = _methods 
	luaL_newlib(L, ssurge::lua_script::luaGameObject_methods);
	lua_setfield(L, meta_id, "__index");

	// metatable.__metatable = _meta 
	luaL_newlib(L, _meta);
	lua_setfield(L, meta_id, "__metatable");

	// class.__metatable = metatable 
	lua_setmetatable(L, lib_id);

	// _G["ssurge"] = newclass 
	lua_setglobal(L, "ssurge");

	return 1;
}

int ssurge::lua_script::ssurge_gc(lua_State* L)
{
	printf("## __gc\n");
	return 0;
}

int ssurge::lua_script::ssurge_newindex(lua_State* L)
{
	printf("## __newindex\n");
	return 0;
}
int ssurge::lua_script::ssurge_index(lua_State* L)
{
	printf("## __index\n");
	return 0;
}