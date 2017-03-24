#include <stdafx.h>
#include <Component.h>
#include <GameObject.h>


std::map<ssurge::ComponentType, std::string> ssurge::Component::mTypeNames;
std::map<std::string, ssurge::ComponentType> ssurge::Component::mOperations;


ssurge::Component::Component(GameObject * owner) : mOwner(owner) 
{  
	
}


std::string ssurge::Component::getComponentTypeNameFromType(ssurge::ComponentType t)
{
	std::map<ComponentType, std::string>::iterator iter;

	iter = mTypeNames.find(t);
	if (iter == mTypeNames.end())
		return mTypeNames[ComponentType::UNKNOWN];
	else
		return iter->second;
}



ssurge::ComponentType ssurge::Component::getComponentTypeFromString(std::string s)
{
	std::map<ComponentType, std::string>::iterator iter;

	for (iter = mTypeNames.begin(); iter != mTypeNames.end(); ++iter)
	{
		if (iter->second == s)
			return iter->first;
	}
	return ComponentType::UNKNOWN;
}


ssurge::ComponentType ssurge::Component::getComponentTypeForOperation(std::string o)
{
	std::map<std::string, ComponentType>::iterator iter;

	iter = mOperations.find(o);
	if (iter == mOperations.end())
		return ComponentType::UNKNOWN;
	else
		return iter->second;
}

bool ssurge::Component::registerOperation(std::string o, ComponentType t)
{
	std::map<std::string, ComponentType>::iterator iter;
	
	iter = mOperations.find(o);
	if (iter == mOperations.end())
	{
		mOperations[o] = t;
		return true;
	}
	return false;
}


void ssurge::Component::initializeComponentMaps()
{
	destroyComponentMaps();

	mTypeNames[ComponentType::UNKNOWN] = "UNKNOWN";
	mTypeNames[ComponentType::MESH] = "MESH";
	mTypeNames[ComponentType::SOUND] = "SOUND";
	mTypeNames[ComponentType::REVERB_ZONE] = "REVERB_ZONE";
	mTypeNames[ComponentType::COLLIDER] = "COLLIDER";
	mTypeNames[ComponentType::RIGIDBODY] = "RIGIDBODY";
}


void ssurge::Component::destroyComponentMaps()
{
	mTypeNames.clear();
	mOperations.clear();
}
