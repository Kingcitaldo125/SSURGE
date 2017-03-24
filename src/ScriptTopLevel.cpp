#include <stdafx.h>
#include <ScriptTopLevel.h>
#include <LogManager.h>
#include <GameObject.h>
#include <GameObjectManager.h>


PyObject * ssurge::script::log(PyObject * self, PyObject * args)
{
	// Check for a single string as the only argument.
	if (!PyTuple_Check(args) || PyTuple_Size(args) != 1 || !PyUnicode_Check(PyTuple_GetItem(args, 0)))
	{
		PyErr_SetString(PyExc_ValueError, "usage: log(string) -> None");
		return NULL;
	}

	// Pick out the string and send it to the log manager.
	char * cstr = PyUnicode_AsUTF8(PyTuple_GetItem(args, 0));
	LOG_MANAGER->log(std::string(cstr), LL_SCRIPT);

	// Return None from the function (in python)
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject * ssurge::script::setBackgroundColor(PyObject * self, PyObject * args)
{

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject * ssurge::script::getAxis(PyObject * self, PyObject * args)
{

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject * ssurge::script::getActionDown(PyObject * self, PyObject * args)
{

	Py_INCREF(Py_None);
	return Py_None;
}

//////////////////////////////////////////////////////////////
// The "table-of-contents" of all module-level functions    //
//////////////////////////////////////////////////////////////
PyMethodDef ssurge_functions[] =
{
	{ "log", ssurge::script::log, METH_VARARGS, "logs a string to the log file" },
	{ "setBackgroundColor", ssurge::script::setBackgroundColor, METH_VARARGS, "sets the viewport's background color" },
	{ "getAxis", ssurge::script::getAxis, METH_VARARGS, "gets the given input axis current value" },
	{ "getActionDown", ssurge::script::getActionDown, METH_VARARGS, "gets the state of the given 'action' binding" },
	{ NULL, NULL, 0, NULL }     // Sentinel
};
