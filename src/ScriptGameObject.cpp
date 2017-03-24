#include <stdafx.h>
#include <ScriptGameObject.h>
#include <GameObject.h>
#include <MeshComponent.h>
#include <CameraComponent.h>
#include <GameObject.h>
#include <GameObjectManager.h>
#include <Tuple.h>
#include <Application.h>

// This method will be called when a ssurge.GameObject needs to be destroyed
// (by the garbage collector)
void ssurge_GameObject_dealloc(PyObject * self)
{
	ssurge::script::ssurge_GameObject * self_go = (ssurge::script::ssurge_GameObject*)self;
	Py_XDECREF(self_go->mCapsule);
	self->ob_type->tp_free((PyObject*)self);
}


// This method will allocate memory for the ssurge.GameObject and do some
// bootstrap initialization
PyObject * ssurge_GameObject_new(PyTypeObject * type, PyObject * args, PyObject * kwds)
{
	ssurge::script::ssurge_GameObject * self_go = (ssurge::script::ssurge_GameObject*)type->tp_alloc(type, 0);
	if (self_go != NULL)
	{
		self_go->mCapsule = NULL;
	}
	else
	{
		PyErr_SetString(PyExc_MemoryError, "Unable to allocate memory for a new ssurge.GameObject");
		return NULL;
	}

	return (PyObject*)self_go;
}


// This method is used to initialize a ssurge.GameObject.  This method *can* be
// called multiple times
int ssurge_GameObject_init(PyObject * self, PyObject * args, PyObject * kwds)
{

	ssurge::script::ssurge_GameObject * self_go = (ssurge::script::ssurge_GameObject*)self;

	if (!PyTuple_Check(args) || PyTuple_Size(args) < 2 || !PyUnicode_Check(PyTuple_GetItem(args, 0)) || !PyLong_Check(PyTuple_GetItem(args, 1)))
	{
		PyErr_SetString(PyExc_ValueError, "You must pass a string name and integer tag in order to instantiate a GameObject");
		return NULL;
	}

	//get the Game Object constructors from args
	char * name = PyUnicode_AsUTF8(PyTuple_GetItem(args, 0));
	int tag = PyLong_AsLong(PyTuple_GetItem(args, 1));

	// Create a C++ game object
	ssurge::GameObject * gobj = GAME_OBJECT_MANAGER->createGameObject(std::string(name), static_cast<unsigned int>(tag));
	self_go->mCapsule = PyCapsule_New(gobj, NULL, NULL);

	// Make the C++ game object aware of its twin
	gobj->setScriptObject(self);

	//call the on create
	if (PyObject_HasAttrString(self, ssurge::script::methods::ON_CREATE))
	{
		PyObject * t = PyTuple_New(0);
		PyObject_Call(PyObject_GetAttrString(self, ssurge::script::methods::ON_CREATE), t, NULL);
		Py_DECREF(t);
	}

	return 0;			// Success!
}

PyObject* ssurge_GameObject_getPos(PyObject* self, PyObject* args, PyObject* kwds)
{
	// Get the C++ game object
	PyObject * capsule = PyObject_GetAttrString(self, "mCapsule");
	ssurge::GameObject * gobj = static_cast<ssurge::GameObject*>(PyCapsule_GetPointer(capsule, NULL));
	Py_DECREF(capsule);

	//get the game object's scene node position
	Ogre::SceneNode* s = gobj->getSceneNode();
	Ogre::Vector3 v = s->_getDerivedPosition();

	//put the position into a pytuple and return it
	PyObject* tuple = PyTuple_New(3);
	PyTuple_SetItem(tuple, 0, PyLong_FromLong(v.x));
	PyTuple_SetItem(tuple, 1, PyLong_FromLong(v.y));
	PyTuple_SetItem(tuple, 2, PyLong_FromLong(v.z));

	return tuple;
}

// This method adds a component to the game object
PyObject* ssurge_GameObject_addComponent(PyObject* self, PyObject* args, PyObject * kwds)
{
	//make sure that the first argument is a string
	if (!PyTuple_Check(args) || !PyUnicode_Check(PyTuple_GetItem(args, 0)))
	{
		PyErr_SetString(PyExc_ValueError, "usage: ssurge.GameObject.addComponent(componentType,args...) -> None");
		return NULL;
	}

	// Get the C++ game object
	PyObject * capsule = PyObject_GetAttrString(self, "mCapsule");
	ssurge::GameObject * gobj = static_cast<ssurge::GameObject*>(PyCapsule_GetPointer(capsule, NULL));
	Py_DECREF(capsule);

	// Get the component type
	char * comType = PyUnicode_AsUTF8(PyTuple_GetItem(args, 0));
	if (std::string(comType).compare("RIGID") == 0)
	{
		//check and make sure the game object has a collider component
		if (gobj->getComponent(ssurge::ComponentType::COLLIDER) != NULL)
		{
			ssurge::RigidBodyComponent* rigCom = new ssurge::RigidBodyComponent(gobj, static_cast<ssurge::ColliderComponent*>(gobj->getComponent(ssurge::ComponentType::COLLIDER))->getShape(), 1);
		}
		else
			LOG_MANAGER->log("RIGID BODY COMPONENTS REQUIRE THE GAME OBJECT TO HAVE A COLLIDER COMPONENT", LL_ERROR);

	}
	else if (std::string(comType).compare("COLLIDER") == 0)
	{
		if (PyUnicode_Check(PyTuple_GetItem(args, 1)))
		{
			//get the shape of the collider
			std::string shapeString = PyUnicode_AsUTF8(PyTuple_GetItem(args, 1));
			ssurge::ColliderComponent* colCom = new ssurge::ColliderComponent(gobj, shapeString);
		}
	}
	else if (std::string(comType).compare("CAMERA")==0)
	{
		//make sure it has a camera type (first, third, fixed)
		if (PyUnicode_Check(PyTuple_GetItem(args, 1)))
		{
			std::vector<ssurge::CameraType> vec;
			std::string cType = PyUnicode_AsUTF8(PyTuple_GetItem(args, 1));
			if (cType.compare("FIRST_PERSON") == 0)
			{
				vec.push_back(ssurge::CameraType::FIRST_PERSON);
				vec.push_back(ssurge::CameraType::THIRD_PERSON);
				vec.push_back(ssurge::CameraType::FIXED);
				ssurge::CameraComponent* camCom = new ssurge::CameraComponent(gobj, vec);
				APPLICATION->setCameraComponent(camCom);
			}
			else if (cType.compare("FIXED") == 0)
			{
				vec.push_back(ssurge::CameraType::FIXED);
				vec.push_back(ssurge::CameraType::THIRD_PERSON);
				vec.push_back(ssurge::CameraType::FIRST_PERSON);
				ssurge::CameraComponent* camCom = new ssurge::CameraComponent(gobj, vec);
				APPLICATION->setCameraComponent(camCom);
			}
			else
			{
				vec.push_back(ssurge::CameraType::THIRD_PERSON);
				vec.push_back(ssurge::CameraType::FIRST_PERSON);
				vec.push_back(ssurge::CameraType::FIXED);
				ssurge::CameraComponent* camCom = new ssurge::CameraComponent(gobj, vec);
				APPLICATION->setCameraComponent(camCom);
			}
		}
	}
	else
		LOG_MANAGER->log(comType, LL_SCRIPT);


	// Return (Python) None
	Py_INCREF(Py_None);
	return Py_None;
}

// This method creates a mesh component on the game object
PyObject * ssurge_GameObject_loadMesh(PyObject * self, PyObject * args, PyObject * kwds)
{
	if (!PyTuple_Check(args) || PyTuple_Size(args) != 1 || !PyUnicode_Check(PyTuple_GetItem(args, 0)))
	{
		PyErr_SetString(PyExc_ValueError, "usage: ssurge.GameObject.loadMesh(string fname) -> None");
		return NULL;
	}

	// Get the C++ game object
	PyObject * capsule = PyObject_GetAttrString(self, "mCapsule");
	ssurge::GameObject * gobj = static_cast<ssurge::GameObject*>(PyCapsule_GetPointer(capsule, NULL));
	Py_DECREF(capsule);

	// Get the string filename
	char * fname = PyUnicode_AsUTF8(PyTuple_GetItem(args, 0));

	// Actually create the mesh component
	ssurge::MeshComponent * meshComp = new ssurge::MeshComponent(gobj);
	meshComp->loadMesh(std::string(fname));

	// Return (Python) None
	Py_INCREF(Py_None);
	return Py_None;
}

//This method uses the Tuple class to pass along data to the C GameObject's do function
PyObject* ssurge_GameObject_handleOperation(PyObject*self, PyObject*args, PyObject*kwds)
{
	// Get the C++ game object
	PyObject * capsule = PyObject_GetAttrString(self, "mCapsule");
	ssurge::GameObject * gobj = static_cast<ssurge::GameObject*>(PyCapsule_GetPointer(capsule, NULL));
	Py_DECREF(capsule);

	//execute the GameObject's handle operation function
	ssurge::Tuple * result = gobj->handleOperation(args);

	if (!result)
		Py_RETURN_FALSE;

	if (result->getSize() == 0)
		Py_RETURN_FALSE;

	// check the success of the operation
	bool success = result->getItemAsBool(0);

	//Return the result of whether or not the handle operation succeeded
	if (!success)
	{
		LOG_MANAGER->log("HANDLE OPERATION FAILED!", LL_ERROR);
		Py_RETURN_FALSE;
	}

	if (result->getSize() != 1)
	{
		PyObject * tuple = result->getPyTuple();

		return tuple;
	}

	Py_RETURN_TRUE;
}
// This method translates the object along the world axes
PyObject * ssurge_GameObject_translateWorld(PyObject * self, PyObject * args, PyObject * kwds)
{
	if (!PyTuple_Check(args) || PyTuple_Size(args) != 3 || !PyNumber_Check(PyTuple_GetItem(args, 0)) ||
		!PyNumber_Check(PyTuple_GetItem(args, 1)) || !PyNumber_Check(PyTuple_GetItem(args, 2)))
	{
		PyErr_SetString(PyExc_ValueError, "You must pass exactly 3 numbers (tx, ty, tz) to this method");
		return NULL;
	}

	// Get the C++ game object
	PyObject * capsule = PyObject_GetAttrString(self, "mCapsule");
	ssurge::GameObject * gobj = static_cast<ssurge::GameObject*>(PyCapsule_GetPointer(capsule, NULL));
	Py_DECREF(capsule);

	PyObject * pnum1 = PyNumber_Float(PyTuple_GetItem(args, 0));
	PyObject * pnum2 = PyNumber_Float(PyTuple_GetItem(args, 1));
	PyObject * pnum3 = PyNumber_Float(PyTuple_GetItem(args, 2));

	double num1 = PyFloat_AsDouble(pnum1);
	double num2 = PyFloat_AsDouble(pnum2);
	double num3 = PyFloat_AsDouble(pnum3);

	Py_DECREF(pnum1);
	Py_DECREF(pnum2);
	Py_DECREF(pnum3);

	gobj->translateWorld(Ogre::Vector3((float)num1, (float)num2, (float)num3));

	// Return (Python) None
	Py_INCREF(Py_None);
	return Py_None;
}



// The method scales the object
PyObject * ssurge_GameObject_scale(PyObject * self, PyObject * args, PyObject * kwds)
{
	if (!PyTuple_Check(args) || PyTuple_Size(args) != 3 || !PyNumber_Check(PyTuple_GetItem(args, 0)) ||
		!PyNumber_Check(PyTuple_GetItem(args, 1)) || !PyNumber_Check(PyTuple_GetItem(args, 2)))
	{
		PyErr_SetString(PyExc_ValueError, "You must pass exactly 3 numbers (sx, sy, sz) to this method");
		return NULL;
	}

	// Get the C++ game object
	PyObject * capsule = PyObject_GetAttrString(self, "mCapsule");
	ssurge::GameObject * gobj = static_cast<ssurge::GameObject*>(PyCapsule_GetPointer(capsule, NULL));
	Py_DECREF(capsule);

	PyObject * pnum1 = PyNumber_Float(PyTuple_GetItem(args, 0));
	PyObject * pnum2 = PyNumber_Float(PyTuple_GetItem(args, 1));
	PyObject * pnum3 = PyNumber_Float(PyTuple_GetItem(args, 2));

	double num1 = PyFloat_AsDouble(pnum1);
	double num2 = PyFloat_AsDouble(pnum2);
	double num3 = PyFloat_AsDouble(pnum3);

	Py_DECREF(pnum1);
	Py_DECREF(pnum2);
	Py_DECREF(pnum3);

	gobj->setScale(Ogre::Vector3((float)num1, (float)num2, (float)num3));

	// Return (Python) None
	Py_INCREF(Py_None);
	return Py_None;
}



// This method rotates the object along the world axes
PyObject * ssurge_GameObject_rotateWorld(PyObject * self, PyObject * args, PyObject * kwds)
{
	if (!PyTuple_Check(args) || PyTuple_Size(args) != 4 || !PyNumber_Check(PyTuple_GetItem(args, 0)) ||
		!PyNumber_Check(PyTuple_GetItem(args, 1)) || !PyNumber_Check(PyTuple_GetItem(args, 2)) ||
		!PyNumber_Check(PyTuple_GetItem(args, 3)))
	{
		PyErr_SetString(PyExc_ValueError, "You must pass exactly 4 numbers (degrees, axisx, axisy, axisz) to this method");
		return NULL;
	}

	// Get the C++ game object
	PyObject * capsule = PyObject_GetAttrString(self, "mCapsule");
	ssurge::GameObject * gobj = static_cast<ssurge::GameObject*>(PyCapsule_GetPointer(capsule, NULL));
	Py_DECREF(capsule);

	PyObject * pnum1 = PyNumber_Float(PyTuple_GetItem(args, 0));
	PyObject * pnum2 = PyNumber_Float(PyTuple_GetItem(args, 1));
	PyObject * pnum3 = PyNumber_Float(PyTuple_GetItem(args, 2));
	PyObject * pnum4 = PyNumber_Float(PyTuple_GetItem(args, 3));

	double num1 = PyFloat_AsDouble(pnum1);
	double num2 = PyFloat_AsDouble(pnum2);
	double num3 = PyFloat_AsDouble(pnum3);
	double num4 = PyFloat_AsDouble(pnum4);

	Py_DECREF(pnum1);
	Py_DECREF(pnum2);
	Py_DECREF(pnum3);
	Py_DECREF(pnum4);

	gobj->rotateWorld((float)num1, Ogre::Vector3((float)num2, (float)num3, (float)num4));

	// Return (Python) None
	Py_INCREF(Py_None);
	return Py_None;
}


// The table-of-contents of all ssurge.GameObject ATTRIBUTES
PyMemberDef ssurge_GameObject_members[] =
{
	{ "mCapsule", T_OBJECT_EX, offsetof(ssurge::script::ssurge_GameObject, mCapsule), 0, "the c-pointer to the C++ game object" },
	{ NULL }				// Sentinel.
};


// The table-of-contents of all ssurge.GameObject METHODS
PyMethodDef ssurge_GameObject_methods[] =
{
	// {"name", (PyCFunction)Noddy_name, METH_NOARGS, "Return the name"},
	{ "loadMesh", (PyCFunction)ssurge_GameObject_loadMesh, METH_VARARGS, "ssurge.GameObject.loadMesh(string fname) -> None"},
	{ "translateWorld", (PyCFunction)ssurge_GameObject_translateWorld, METH_VARARGS, "ssurge.GameObject.translateWorld(x,y,z) -> None" },
	{ "scale", (PyCFunction)ssurge_GameObject_scale, METH_VARARGS, "ssurge.GameObject.scale(sx,sy,sz) -> None"},
	{ "rotateWorld", (PyCFunction)ssurge_GameObject_rotateWorld, METH_VARARGS, "ssurge.GameObject.rotateWorld(degrees, axisX,axisY,axisZ)" },
	{"handleOperation",(PyCFunction)ssurge_GameObject_handleOperation,METH_VARARGS,"ssurge.GameObject.handleOperation(function,arg1,arg2,...)"},
	{"addComponent",(PyCFunction)ssurge_GameObject_addComponent,METH_VARARGS,"ssurge.GameObject.addComponent(componentType, arg1, arg2,...)"},
	{"getPos",(PyCFunction)ssurge_GameObject_getPos,METH_VARARGS,"ssurge.GameObject.getPos()"},
	{ NULL }				// Sentinel.
};

// This is the master list of ssurge.GameObject methods
PyTypeObject ssurge_GameObject_Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"ssurge.GameObject",											/* tp_name */
	sizeof(ssurge::script::ssurge_GameObject),						/* tp_basicsize */
	0,																/* tp_itemsize */
	(destructor)ssurge_GameObject_dealloc,							/* tp_dealloc */
	0,																/* tp_print */
	0,																/* tp_getattr */
	0,																/* tp_setattr */
	0,																/* tp_reserved */
	0,																/* tp_repr */
	0,																/* tp_as_number */
	0,																/* tp_as_sequence */
	0,																/* tp_as_mapping */
	0,																/* tp_hash  */
	0,																/* tp_call */
	0,																/* tp_str */
	0,																/* tp_getattro */
	0,																/* tp_setattro */
	0,																/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,						/* tp_flags */
	"A Python wrapper around our C++ GameObject",					/* tp_doc */
	0,																/* tp_traverse */
	0,																/* tp_clear */
	0,																/* tp_richcompare */
	0,																/* tp_weaklistoffset */
	0,																/* tp_iter */
	0,																/* tp_iternext */
	ssurge_GameObject_methods,										/* tp_methods */
	ssurge_GameObject_members,										/* tp_members */
	0,																/* tp_getset */
	0,																/* tp_base */
	0,																/* tp_dict */
	0,																/* tp_descr_get */
	0,																/* tp_descr_set */
	0,																/* tp_dictoffset */
	(initproc)ssurge_GameObject_init,								/* tp_init */
	0,																/* tp_alloc*/
	ssurge_GameObject_new,											/* tp_new */
};



