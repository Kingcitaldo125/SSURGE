#ifndef _SCRIPT_MAIN_H_
#define _SCRIPT_MAIN_H_

#include <stdafx.h>

namespace ssurge
{
	namespace script
	{
		/// The function that initializes and returns the "ssurge" module
		PyObject * PyInit_ssurge(void);
	}
}

#endif
