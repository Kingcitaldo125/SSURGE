#include <stdafx.h>
#include <ScriptTopLevel.h>
#include <ScriptMain.h>
#include <ScriptManager.h>

// A reference to the ssurge_GameObject_Type structure (the big one) defined in ScriptGameObject.cpp
extern PyTypeObject ssurge_GameObject_Type;

// A reference to the ssurge top-level function table-of-contents
extern PyMethodDef ssurge_functions[];


//////////////////////////////////////////////////////////////
// The module definition itself                             //
//////////////////////////////////////////////////////////////
struct PyModuleDef ssurge_definition =
{
	PyModuleDef_HEAD_INIT,
	"ssurge",					    // Name of module.  Doesn't *have* to be the
	"My super-awesome first python extension",		// Can look at (in python) with print(testMod.__doc__)
	-1,								// -1 if the module keeps its state in global variables
	ssurge_functions, NULL, NULL, NULL, NULL
};


//////////////////////////////////////////////////////////////
// The module initialization function                       //
//////////////////////////////////////////////////////////////
// Note: This function *must* be called PyInit_XXX where XXX is the module
// name.  This function will be passed to PyImport_AppendInitTabs.
PyObject * ssurge::script::PyInit_ssurge(void)
{
	// Finish "filling in" the slots in ssurge_GameObject_Type
	if (PyType_Ready(&ssurge_GameObject_Type) < 0)
		return NULL;

	// Create our module 
	PyObject * mod = PyModule_Create(&ssurge_definition);
	if (mod == NULL)
	{
		PyErr_SetString(PyExc_ImportError, "Error initializing ssurge module");
		return NULL;
	}

	// This adds the new GameObject type to ssurge.  I'm honestly not sure if the
	// INCREF is necessary or not.  I definitely don't want the gameobject class
	// to go away, but is it safe to assume that mod will stay in existence *after*
	// someone imports it and is done with it?  To be safe, I'll INCREF for now -- 
	// keep an eye on this...
	Py_INCREF(&ssurge_GameObject_Type);
	PyModule_AddObject(mod, "GameObject", (PyObject*)&ssurge_GameObject_Type);

	SCRIPT_MANAGER->ssurgeModule = mod;
	// Return the new module to the interpreter (it's now available for
	// "importing")
	return mod;
}