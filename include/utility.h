#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <stdafx.h>

namespace ssurge
{
	/// Generates a random float between the given min and max values.
	float rand_float(float min, float max);

	void find_fileType(std::vector<std::string>& v, std::string extension);

	// J Li
	// Using MaterialManager as how MadMarx used it
	// http://www.ogre3d.org/tikiwiki/tiki-index.php?page=MadMarx+Tutorial+7
	Ogre::Pass* createMaterial(std::string matName, std::string directory = "");

	void setLightingOn(Ogre::Pass* pass);

	void setMatDiffuse(Ogre::Pass* pass, Ogre::ColourValue diffuse);

	void setMatAmbient(Ogre::Pass* pass, Ogre::ColourValue ambient);

	void setMatSpecular(Ogre::Pass* pass, Ogre::ColourValue specular);

	void setMatEmissive(Ogre::Pass* pass, Ogre::ColourValue emissive);

	void setMatShininess(Ogre::Pass* pass, Ogre::Real shininess);

	void setMatTexture(Ogre::Pass* pass, std::string imgFileName);

	void setMaterial(Ogre::Entity* ent, std::string matName);
}

#endif