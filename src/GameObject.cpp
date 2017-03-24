#include <stdafx.h>
#include <GameObject.h>
#include <Application.h>
#include <MeshComponent.h>

ssurge::GameObject::GameObject(std::string name, unsigned int tag) : mTag(tag), mName(name), mScriptObject(NULL), mComponentOperationFailure(false)
{
	mSceneNode = APPLICATION->getSceneManager()->getRootSceneNode()->createChildSceneNode(name + "_node");
	mSceneNode->setListener(this);
}


ssurge::GameObject::GameObject(std::string name, unsigned int tag, ssurge::GameObject * parent) : mTag(tag), mName(name), mComponentOperationFailure(false)
{
	mSceneNode = parent->mSceneNode->createChildSceneNode(name + "_node");
	mSceneNode->setListener(this);
}

ssurge::GameObject::~GameObject()
{
	// Remove and destroy all components.  I'm assuming when addComponent is called,
	// this GameObject takes memory ownership of that component and is responsible for destroy it (here)
	std::map<ComponentType, Component*>::iterator iter;
	for (iter = mComponents.begin(); iter != mComponents.end(); ++iter)
		delete iter->second;
	mComponents.clear();

	// Remove and destroy all attached objects.  This will eventually be moved to the
	// components that own these ogre objects.  For now, just destroy cameras and lights.
	Ogre::SceneNode::ObjectIterator attach_iter = mSceneNode->getAttachedObjectIterator();
	while (attach_iter.hasMoreElements())
	{
		Ogre::MovableObject * obj = attach_iter.getNext();
		std::string tp = obj->getMovableType();
		if (tp == "Camera" || tp == "Light")
		{
			APPLICATION->getSceneManager()->destroyMovableObject(obj);
		}
	}

	// Remove this node from its parent and destroy it
	mSceneNode->getParentSceneNode()->removeChild(mSceneNode);
	APPLICATION->getSceneManager()->destroySceneNode(mSceneNode);
}

void ssurge::GameObject::addComponent(ssurge::Component * c)
{
	// Currently, we're only supporting one component of each type.  So here,
	// I'm checking to see if we already have one of that type.  If so, I'll
	// destroy it (so we don't get a memory leak).  DestroyComponent has no effect
	// if there is no component of type ctype.
	ComponentType ctype = c->getType();
	destroyComponent(ctype);

	// Finally, insert the component into our dictionary
	mComponents[ctype] = c;
}

ssurge::Component * ssurge::GameObject::removeComponent(ComponentType ctype)
{
	std::map<ComponentType, Component*>::iterator iter;
	iter = mComponents.find(ctype);
	if (iter != mComponents.end())
	{
		Component * comp = iter->second;
		mComponents.erase(iter);
		return comp;
	}
	return NULL;
}

void ssurge::GameObject::destroyComponent(ComponentType ctype)
{
	Component * comp = removeComponent(ctype);
	if (comp != NULL)
	{
		delete comp;
		comp = NULL;
	}
}

int ssurge::GameObject::addCamera(Ogre::Camera * c, bool make_active)
{
	mSceneNode->attachObject(c);
	if (make_active)
	{
		APPLICATION->setActiveCamera(c);
	}
	return 0;
}

int ssurge::GameObject::addLight(Ogre::Light * L)
{
	mSceneNode->attachObject(L);
	return 0;
}

void ssurge::GameObject::removeChild(unsigned int index)
{
	unsigned int num_children = mSceneNode->numChildren();
	if (num_children > index)
	{
		mSceneNode->removeChild(index);
	}
}

void ssurge::GameObject::removeChild(GameObject * g)
{
	Ogre::Node * node = mSceneNode->getChild(g->mSceneNode->getName());
	if (node != NULL)
		mSceneNode->removeChild(node);
}

void ssurge::GameObject::addChild(ssurge::GameObject * go)
{
	Ogre::SceneNode * parent = go->mSceneNode->getParentSceneNode();
	if (parent)
		parent->removeChild(go->mSceneNode);
	mSceneNode->addChild(go->mSceneNode);
}

ssurge::Tuple * ssurge::GameObject::handleOperation(PyObject * args)
{
	if (!PyTuple_Check(args) || PyTuple_Size(args) == 0 || !PyUnicode_Check(PyTuple_GetItem(args, 0)))
	{
		return false;
	}
	unsigned int s = PyTuple_Size(args);
	std::string o = std::string(PyUnicode_AsUTF8(PyTuple_GetItem(args, 0)));
	if (s == 1)
		return handleOperation(o, new Tuple());
	PyObject * t = PyTuple_New(s - 1);
	for (unsigned int i = 1; i != s; ++i)
	{
		PyTuple_SetItem(t, i - 1, PyTuple_GetItem(args, i));
	}
	return handleOperation(o, new Tuple(t));
}

ssurge::Tuple * ssurge::GameObject::handleOperation(std::string o, Tuple * t)
{
	ComponentType ct = Component::getComponentTypeForOperation(o);
	if (ct == ComponentType::UNKNOWN)
		return NULL;
	
	std::map<ComponentType, Component*>::iterator iter;
	iter = mComponents.find(ct);
	if (iter == mComponents.end())
		return NULL;

	mComponentOperationFailure = false;
	Tuple * result = iter->second->handleOperation(o, t);

	return new Tuple(result, "b", mComponentOperationFailure);
}

void ssurge::GameObject::setScriptObject(PyObject * twin)
{
	if (mScriptObject != NULL)
		removeScriptObject();
	mScriptObject = twin;
}

void ssurge::GameObject::removeScriptObject()
{

}

void ssurge::GameObject::update(double dt)
{
	/*for (std::map<ComponentType, Component*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
	{
		if (it->second)
		{
			it->second->update(dt);
		}
	}*/

	/*if (mScriptObject != NULL)
	{
		if (PyObject_HasAttrString(mScriptObject, "update"))
		{
			PyObject* args = PyTuple_New(1);
			PyTuple_SetItem(args, 0, PyFloat_FromDouble(dt));
			PyObject_Call(PyObject_GetAttrString(mScriptObject, "update"), args, NULL);
			Py_DECREF(args);
		}
	}*/
}

void ssurge::GameObject::nodeUpdated(const Ogre::Node* n)
{
	std::map<ssurge::ComponentType, Component*>::iterator iter = mComponents.begin();
	while (iter != mComponents.end())
	{
		iter->second->ownerMoved(n);
		++iter;
	}
}

void ssurge::GameObject::setPythonKwarg(PyObject * key, PyObject * arg) {
	PyDict_SetItem(mPythonKwargs, key, arg);
}

void ssurge::GameObject::setPythonArgs(PyObject * args) {
	if (PyTuple_Check(args)) {
		mPythonArgs = args;
	}
	else {
		LOG_MANAGER->log("Argument passed to setPythonArgs of GameObject was not a tuple", LL_WARNING);
	}
}