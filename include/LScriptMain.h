#ifndef _LUA_SCRIPT_MAIN_H_
#define _LUA_SCRIPT_MAIN_H_

#include <stdafx.h>
#include <LuaScriptTopLevel.h>
#include <LogManager.h>
#include <Component.h>

namespace ssurge
{
	namespace lua_script
	{
		typedef struct LuaGameObject
		{
			/// The Gameobject being manipulated by the lua scripts
			GameObject * mGameObject;

			/// The scene node containing the transformation data (and all attached Ogre objects)
			Ogre::SceneNode * mSceneNode;

			/// The "tag" of this game object.  This is a user-defined value (could be used to mark
			/// players, enemies, level0 objects, etc.)
			unsigned int mTag;

			/// The name of the game object.  This is the key in GameObjectManager's master list of all objects
			std::string mName;

			/// A dictionary of components.  Note: for now, we only allow once component of each type.  If this
			/// design goal changes, we could make this a dictionary of linked-lists (or equivalent)
			std::map<ComponentType, Component*> mComponents;
		} LuaGameObject;
			
		int ssurge_gc(lua_State *L);
		int ssurge_index(lua_State *L);
		int ssurge_newindex(lua_State *L);

		static const struct luaL_Reg _meta[] = 
		{
			{ "__gc", ssurge::lua_script::ssurge_gc },
			{ "__index", ssurge::lua_script::ssurge_index },
			{ "__newindex", ssurge::lua_script::ssurge_newindex },
			{ NULL, NULL }
		};

		static const struct luaL_Reg luaGameObject_methods[] =
		{
			{"log", ssurge::lua_script::log},
			{"setBackgroundColor", ssurge::lua_script::setBackgroundColor},
			{"getAxis", ssurge::lua_script::getAxis},
			{"getActionDown", ssurge::lua_script::getActionDown},
			{ NULL, NULL }
		};

		int luaopen_ssurge(lua_State *L);
	}
}

#endif
