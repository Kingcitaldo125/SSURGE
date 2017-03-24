#include <stdafx.h>
#include <GameObjectManager.h>
#include <GameObject.h>
#include <Application.h>
#include <SimplePlayer.h>
#include <MeshComponent.h>
#include <CameraComponent.h>
#include <ScriptManager.h>
#include <ScriptGameObject.h>
#include <ColliderComponent.h>
#include <RigidBodyComponent.h>

template<>
ssurge::GameObjectManager * ssurge::Singleton<ssurge::GameObjectManager>::msSingleton = nullptr;


bool ssurge::GameObjectManager::DataWalker::for_each(pugi::xml_node & node)
{
	if (!strcmp(node.name(), "node"))
	{
		std::string name = findName(node); 
		int tag = findTag(node);
		GameObject * parent = findParent(node);
		// <lab17 modifications>
		bool isPlayer = false;
		bool hasClass = false;

		pugi::xml_attribute pythonClass = node.attribute("class");
		if (!pythonClass.empty() && pythonClass.as_string())
		{
			hasClass = true;
		}
		pugi::xml_attribute attrib = node.attribute("isPlayer");
		if (!attrib.empty() && attrib.as_bool())
			isPlayer = true;

		// We can now create the (empty) game object.
		GameObject * gobj;


		//if there is a script class associated with this object, then create that via script and set our empty gobj to the game object that was created

		if (isPlayer)
		{
			gobj = new SimplePlayer(name, tag);
			LOG_MANAGER->log(gobj->getSceneNode()->getName(), LL_DEBUG);
			std::cout << gobj->getSceneNode()->getName() << std::endl;
		}
		else if (hasClass)
		{
			PyObject * t = PyTuple_New(2);
			PyObject * d = PyModule_GetDict(SCRIPT_MANAGER->ssurgeModule);
			PyObject * s = PyUnicode_FromString(pythonClass.as_string());
			PyTuple_SetItem(t, 0, PyUnicode_FromString(name.c_str()));
			PyTuple_SetItem(t, 1, PyLong_FromLong(tag));
			PyObject_CallObject(PyDict_GetItem(d, s), t);
			gobj = mManager->findGameObject(name);

			Py_DECREF(t);
			Py_DECREF(s);
		}
		else if (parent != NULL)
		{
			gobj = new GameObject(name, tag, parent);
		}
		else {
			gobj = new GameObject(name, tag);
		}
		// </lab17 modifications>
		
		// Process any transformations and components
		processTransforms(node, gobj);
		processMesh(node, gobj);
		processLight(node, gobj);
		processCamera(node, gobj);
		processCollider(node, gobj);
		

		// Add this game object to the master list of game objects.  Just to make sure there are no
		// errors, I first check to see if the list contains that entry already -- if so, free up its
		// memory and add the new game object
		if (!hasClass)
		{
			std::map<std::string, GameObject*>::iterator iter = mManager->mObjects.find(name);
			if (iter != mManager->mObjects.end())
			{
				delete iter->second;
				iter->second = NULL;
			}
		}
		mManager->mObjects[name] = gobj;

		// TODO: call on Create with kwargs
		if (gobj->mScriptObject != NULL && PyObject_HasAttrString(gobj->mScriptObject, ssurge::script::methods::ON_XML))
		{
			PyObject * t = PyTuple_New(0);
			PyObject_Call(PyObject_GetAttrString(gobj->mScriptObject, ssurge::script::methods::ON_XML), t, gobj->mPythonKwargs);
			Py_DECREF(t);
		}

	}
	else if (!strcmp(node.name(), "environment"))
	{
		pugi::xml_attribute attrib;
		pugi::xml_node temp = node.child("colourBackground");
		if (!temp.empty())
			APPLICATION->setBackgroundColor(getColour(temp));
		temp = node.child("colourAmbient");
		if (!temp.empty())
			APPLICATION->getSceneManager()->setAmbientLight(getColour(temp));
	}
	
	return true;
}


ssurge::GameObject * ssurge::GameObjectManager::DataWalker::findParent(pugi::xml_node & node)
{
	pugi::xml_node parent = node.parent();
	if (parent.empty())
		return NULL;
	else
	{
		std::string parentName = findName(parent);
		return mManager->findGameObject(parentName);
	}
}


std::string ssurge::GameObjectManager::DataWalker::findName(pugi::xml_node & node)
{
	return std::string(node.attribute("name").as_string());
}



int ssurge::GameObjectManager::DataWalker::findTag(pugi::xml_node & node)
{
	int tag = 0;
	pugi::xml_node temp = node.child("user_data");
	if (!temp.empty())
	{
		pugi::xml_attribute attrib = temp.attribute("name");
		if (!attrib.empty() && !strcmp(attrib.as_string(), "tag"))
			tag = temp.attribute("value").as_int();
	}
	return tag;
}



void ssurge::GameObjectManager::DataWalker::processTransforms(pugi::xml_node & node, ssurge::GameObject * gobj)
{
	pugi::xml_attribute attrib;
	pugi::xml_node temp = node.child("position");
	if (!temp.empty()) 
	{
		Ogre::Vector3 pos = getVector3(temp);
		gobj->setPosition(pos);
		gobj->setPythonKwarg(PyUnicode_FromString("position"), Py_BuildValue("(ddd)", pos[0], pos[1], pos[2]));
	}
	
	temp = node.child("rotation");
	if (!temp.empty()) 
	{
		Ogre::Quaternion quat = getQuaternion(temp);
		gobj->setOrientationQuaternion(getQuaternion(temp));
		gobj->setPythonKwarg(PyUnicode_FromString("rotation"), Py_BuildValue("(dddd)", quat[0], quat[1], quat[2], quat[3]));
	}
	
	temp = node.child("scale");
	if (!temp.empty()) 
	{
		Ogre::Vector3 scale = getVector3(temp);
		gobj->setScale(scale);
		gobj->setPythonKwarg(PyUnicode_FromString("scale"), Py_BuildValue("(ddd)", scale[0], scale[1], scale[2]));
	}
}



void ssurge::GameObjectManager::DataWalker::processMesh(pugi::xml_node & node, ssurge::GameObject * gobj)
{
	pugi::xml_node temp = node.child("entity");
	if (!temp.empty())
	{
		// Note: The name of the mesh file is the same as the game object name.
		MeshComponent * mc = new MeshComponent(gobj);
		mc->loadMesh(temp.attribute("meshFile").as_string());
		gobj->setPythonKwarg(PyUnicode_FromString("meshfile"), PyUnicode_FromString(temp.attribute("meshFile").as_string()));
	}
}



void ssurge::GameObjectManager::DataWalker::processLight(pugi::xml_node & node, ssurge::GameObject * gobj)
{
	pugi::xml_attribute attrib;
	pugi::xml_node temp = node.child("light");
	pugi::xml_node sub_node;
	if (!temp.empty())
	{
		// Determine if this is a point, direction, or spot-light
		attrib = temp.attribute("type");
		if (attrib.empty())
			return;

		Ogre::Light * L = NULL;
		if (!strcmp(attrib.as_string(), "directional"))
		{
			L = APPLICATION->getSceneManager()->createLight(gobj->getName());
			L->setType(Ogre::Light::LT_DIRECTIONAL);
			
		}
		else if (!strcmp(attrib.as_string(), "spot"))
		{
			L = APPLICATION->getSceneManager()->createLight(gobj->getName());
			L->setType(Ogre::Light::LT_SPOTLIGHT);
		}
		else if (!strcmp(attrib.as_string(), "point"))
		{
			L = APPLICATION->getSceneManager()->createLight(gobj->getName());
			L->setType(Ogre::Light::LT_POINT);
		}
		
		if (L == NULL)
		{
			// Unknown type of light (or missing piece of critical information) -- return without creating it.
			return;
		}

		// See if this light has a direction
		sub_node = temp.child("direction");
		if (!sub_node.empty())
			L->setDirection(getVector3(sub_node));

		// See if this light has spotlight values
		sub_node = temp.child("spotLightRange");
		if (!sub_node.empty())
		{
			attrib = sub_node.attribute("falloff");
			if (!attrib.empty())		
				L->setSpotlightFalloff(attrib.as_float());
			attrib = sub_node.attribute("inner");
			if (!attrib.empty())		
				L->setSpotlightInnerAngle(Ogre::Radian(attrib.as_float()));
			attrib = sub_node.attribute("outer");
			if (!attrib.empty())		
				L->setSpotlightOuterAngle(Ogre::Radian(attrib.as_float()));	
		}

		// See if this light should cast shadows
		attrib = temp.attribute("shadow");
		if (!attrib.empty() && !strcmp(attrib.as_string(), "true"))
			L->setCastShadows(true);
		else
			L->setCastShadows(false);

		// See if we have a diffuse or specular color.
		sub_node = temp.child("colourDiffuse");
		if (!sub_node.empty())
			L->setDiffuseColour(getColour(sub_node));
		sub_node = temp.child("colourSpecular");
		if (!sub_node.empty())
			L->setSpecularColour(getColour(sub_node));
		
		// See if we have attenuation values.
		sub_node = temp.child("lightAttenuation");
		if (!sub_node.empty())
		{
			float c = 1.0f, r = 100.0f, l = 0.0f, q = 0.0f;
			attrib = sub_node.attribute("constant");
			if (!attrib.empty())		c = attrib.as_float();
			attrib = sub_node.attribute("range");
			if (!attrib.empty())		r = attrib.as_float();
			attrib = sub_node.attribute("linear");
			if (!attrib.empty())		l = attrib.as_float();
			attrib = sub_node.attribute("quadratic");
			if (!attrib.empty())		q = attrib.as_float();
			L->setAttenuation(r, c, l, q);
		}

		gobj->addLight(L);
	}
}



void ssurge::GameObjectManager::DataWalker::processCamera(pugi::xml_node & node, ssurge::GameObject * gobj)
{
	// Might be a janky implementation of the CameraComponent, poor planning on my part :/

	pugi::xml_node temp = node.child("camera");
	if (!temp.empty())
	{
		///@PA:I changed a couple camera component parameters
		std::vector<ssurge::CameraType> vec;
		vec.push_back(ssurge::CameraType::FIRST_PERSON);
		vec.push_back(ssurge::CameraType::THIRD_PERSON);
		CameraComponent * cc = new CameraComponent(gobj, vec);
		Ogre::Camera * c = APPLICATION->getSceneManager()->createCamera(gobj->getName());
		c->setAspectRatio(APPLICATION->getAspectRatio());
		pugi::xml_attribute attrib = temp.attribute("fov");
		if (!attrib.empty())
		{
			c->setFOVy(Ogre::Radian(attrib.as_float()));
		}
		temp = temp.child("clipping");
		if (!temp.empty())
		{
			attrib = temp.attribute("far");
			if (!attrib.empty())
				c->setFarClipDistance(attrib.as_float());
			attrib = temp.attribute("near");
			if (!attrib.empty())
				c->setNearClipDistance(attrib.as_float());
		}
		
		cc->setCamera(c);
		gobj->addCamera(cc->getCamera(), gobj->getTag() == mCameraTag);

		///@PA: pass the camera component to the application.
		APPLICATION->setCameraComponent(cc);
	}
}

void ssurge::GameObjectManager::DataWalker::processCollider(pugi::xml_node & node, ssurge::GameObject * gobj)
{
	pugi::xml_attribute attrib;
	pugi::xml_node temp = node.child("collider");
	if (!temp.empty())
	{
		std::string shape = "box";
		attrib = temp.attribute("shape");
		if (!attrib.empty())
		{
			shape = attrib.as_string();
		}
		float mass = 0;
		attrib = temp.attribute("mass");
		if (!attrib.empty())
		{
			mass = attrib.as_float();
		}
		ColliderComponent * cComp = new ColliderComponent(gobj, shape);
		RigidBodyComponent * rbComp = new RigidBodyComponent(gobj, cComp->getShape(), mass);
		
		attrib = temp.attribute("friction");
		if (!attrib.empty())
		{
			float friction = attrib.as_float();
			rbComp->getRigidBody()->setFriction(friction);
			rbComp->getRigidBody()->setRollingFriction(friction);
		}
		attrib = temp.attribute("restitution");
		if (!attrib.empty())
		{
			float restitution = attrib.as_float();
			rbComp->getRigidBody()->setRestitution(restitution);
		}
	}
}

Ogre::Vector3 ssurge::GameObjectManager::DataWalker::getVector3(pugi::xml_node & node)
{
	float x = 0.0f, y = 0.0f, z = 0.0f;
	pugi::xml_attribute attrib = node.attribute("x");
	if (!attrib.empty())		x = attrib.as_float();
	attrib = node.attribute("y");
	if (!attrib.empty())		y = attrib.as_float();
	attrib = node.attribute("z");
	if (!attrib.empty())		z = attrib.as_float();
	return Ogre::Vector3(x, y, z);
}


Ogre::ColourValue ssurge::GameObjectManager::DataWalker::getColour(pugi::xml_node & node)
{
	float r = 0.0f, g = 0.0f, b = 0.0f;
	pugi::xml_attribute attrib = node.attribute("r");
	if (!attrib.empty())		r = attrib.as_float();
	attrib = node.attribute("g");
	if (!attrib.empty())		g = attrib.as_float();
	attrib = node.attribute("b");
	if (!attrib.empty())		b = attrib.as_float();
	return Ogre::ColourValue(r, g, b);
}


Ogre::Quaternion ssurge::GameObjectManager::DataWalker::getQuaternion(pugi::xml_node & node)
{
	float w = 1.0f, x = 0.0f, y = 0.0f, z = 0.0f;
	pugi::xml_attribute attrib = node.attribute("qw");
	if (!attrib.empty())		w = attrib.as_float();
	attrib = node.attribute("qx");
	if (!attrib.empty())		x = attrib.as_float();
	attrib = node.attribute("qy");
	if (!attrib.empty())		y = attrib.as_float();
	attrib = node.attribute("qz");
	if (!attrib.empty())		z = attrib.as_float();
	return Ogre::Quaternion(w, x, y, z);
}

int ssurge::GameObjectManager::loadXML(std::string fname, unsigned int cameraTag)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(fname.c_str());

	if (result)
	{
		DataWalker walker(this, cameraTag);
		doc.traverse(walker);
		return 0;
	}
	else
	{
		LOG_MANAGER->log("Unable to load xml file '" + fname + "'!", LL_ERROR);
		return 1;
	}
}

void ssurge::GameObjectManager::destroyAll()
{
	std::map<std::string, GameObject*>::iterator iter = mObjects.begin();
	while (iter != mObjects.end())
	{
		delete iter->second;
		iter->second = NULL;
	}
	mObjects.clear();
}


ssurge::GameObject * ssurge::GameObjectManager::findGameObject(std::string name)
{
	std::map<std::string, GameObject*>::iterator iter = mObjects.find(name);
	if (iter == mObjects.end())
		return NULL;
	else
		return iter->second;
}


void ssurge::GameObjectManager::update(double dt)
{
	std::map<std::string, GameObject*>::iterator iter = mObjects.begin();
	while (iter != mObjects.end())
	{
		iter->second->update(dt);

		/*iter->second->mComponents.begin()
		for (int i = 0; i < )*/

		// ` GameObject's Components
		for (std::map<ComponentType, Component*>::iterator itComp = iter->second->mComponents.begin(); itComp != iter->second->mComponents.end(); ++itComp)
		{
			if (itComp->second)
			{
				itComp->second->update(dt);
			}
		}

		//Update scripts
		if (iter->second->mScriptObject != NULL)
		{
			if (PyObject_HasAttrString(iter->second->mScriptObject, ssurge::script::methods::UPDATE))
			{
				PyObject* args = PyTuple_New(1);
				PyTuple_SetItem(args, 0, PyFloat_FromDouble(dt));
				PyObject_Call(PyObject_GetAttrString(iter->second->mScriptObject, ssurge::script::methods::UPDATE), args, NULL);
				Py_DECREF(args);
			}
		}

		iter++;
	}
}