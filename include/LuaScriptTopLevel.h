#include <stdafx.h>

namespace ssurge
{
	/// This nested namespace contains prototypes for any top-level
	/// Python/C functions that are to be included in the "ssurge" Python/C module
	namespace lua_script
	{
		/// Writes a string to the log (using LL_SCRIPT).
		int log(lua_State *L);

		/// Sets the main window's background color.
		int setBackgroundColor(lua_State *L);

		/// Gets the current value of an input axis.
		int getAxis(lua_State *L);

		/// Gets the state of the given action button.
		int getActionDown(lua_State *L);
		
	}
}