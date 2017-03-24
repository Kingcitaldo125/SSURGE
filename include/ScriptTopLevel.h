#include <stdafx.h>

namespace ssurge
{
	/// This nested namespace contains prototypes for any top-level
	/// Python/C functions that are to be included in the "ssurge" Python/C module
	namespace script
	{
		/// Writes a string to the log (using LL_SCRIPT).  Python-usage: ssurge.log(string)
		PyObject * log(PyObject * self, PyObject * args);

		/// Sets the main window's background color.  Python-usage: ssurge.setBackgroundColor(r, g, b)
		PyObject * setBackgroundColor(PyObject * self, PyObject * args);

		/// Gets the current value of an input axis.  Python-usage: ssurge.getAxis(int axis_num, bool is_horiz) -> float
		PyObject * getAxis(PyObject * self, PyObject * args);

		/// Gets the state of the given action button.  Python-usage: ssurge.getActionDown(string action_name) -> boolean
		PyObject * getActionDown(PyObject * self, PyObject * args);
	}
}