#ifndef _SCRIPT_GAME_OBJECT_H_
#define _SCRIPT_GAME_OBJECT_H_

#include <stdafx.h>

namespace ssurge
{
	namespace script
	{
		namespace methods {

			//methods that can be defined in gameobject python script
			static const char * ON_CREATE = "onCreate";
			static const char * UPDATE = "update";
			static const char * ON_XML = "onXML";
		}
		// Reference: https://docs.python.org/3.5/extending/newtypes.html
		typedef struct
		{
			PyObject_HEAD
				PyObject * mCapsule;
		}ssurge_GameObject;
		
	}
}

#endif
