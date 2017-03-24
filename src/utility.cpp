#include <stdafx.h>
#include <utility.h>
#include <LogManager.h>

float ssurge::rand_float(float min, float max)
{
	double v;

	v = (max - min) * (double)rand() / RAND_MAX + min;

	return (float)v;
}

void ssurge::find_fileType(std::vector<std::string>& v,std::string extension)
{
	//example usage: ssurge::find_fileType(stringVec*, "*.py");
	//find all files of the specified extension
	Ogre::StringVectorPtr pyFiles = Ogre::ResourceGroupManager::getSingleton().findResourceNames(Ogre::ResourceGroupManager::getSingleton().DEFAULT_RESOURCE_GROUP_NAME, extension);
	
	for (Ogre::StringVector::iterator pyIt = pyFiles->begin(); pyIt != pyFiles->end(); pyIt++)
	{
		//push all of those files into the specified vector
		v.push_back(pyIt->c_str());
	}
	
	return;
}

Ogre::Pass* ssurge::createMaterial(std::string matName, std::string directory)
{
	Ogre::MaterialManager& lMaterialManager = Ogre::MaterialManager::getSingleton();
	std::string groupName = matName + "Group";

	Ogre::ResourceGroupManager& lRgMgr = Ogre::ResourceGroupManager::getSingleton();

	lRgMgr.createResourceGroup(groupName);
	lRgMgr.initialiseResourceGroup(groupName);

	Ogre::MaterialPtr lMaterial = lMaterialManager.create(matName, groupName);
	Ogre::Technique* lFirstTechnique = lMaterial->getTechnique(0);
	Ogre::Pass* lFirstPass = lFirstTechnique->getPass(0);

	lFirstPass->setLightingEnabled(false);

	return lFirstPass;
}

void ssurge::setLightingOn(Ogre::Pass* pass)
{
	pass->setLightingEnabled(true);
}

void ssurge::setMatDiffuse(Ogre::Pass* pass, Ogre::ColourValue diffuse)
{
	pass->setDiffuse(diffuse);
}

void ssurge::setMatAmbient(Ogre::Pass* pass, Ogre::ColourValue ambient)
{
	pass->setAmbient(ambient);
}

void ssurge::setMatSpecular(Ogre::Pass* pass, Ogre::ColourValue specular)
{
	pass->setSpecular(specular);
}

void ssurge::setMatEmissive(Ogre::Pass* pass, Ogre::ColourValue emissive)
{
	pass->setEmissive(emissive);
}

void ssurge::setMatShininess(Ogre::Pass* pass, Ogre::Real shininess)
{
	pass->setShininess(shininess);
}

void ssurge::setMatTexture(Ogre::Pass* pass, std::string imgFileName)
{
	Ogre::TextureUnitState* tmp = pass->createTextureUnitState();
	tmp->setTextureName(imgFileName);
}

void ssurge::setMaterial(Ogre::Entity* ent, std::string matName)
{
	ent->setMaterialName(matName);
}
