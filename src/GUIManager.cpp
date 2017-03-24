#include <stdafx.h>
#include <GUIManager.h>


template<>
ssurge::GUIManager * ssurge::Singleton<ssurge::GUIManager>::msSingleton = nullptr;

ssurge::GUIManager::GUIManager()
{
	// This is a pain, but it appears as if the font isn't properly loaded unless you manually load it this
	// way...check into this.
	Ogre::FontManager * fmgr = Ogre::FontManager::getSingletonPtr();
	fmgr->getByName("MyFont")->load();

	// Reference: http://www.ogre3d.org/tikiwiki/Creating+Overlays+via+Code
	omgr = Ogre::OverlayManager::getSingletonPtr();

	Ogre::Overlay * pover = omgr->create("TestOverlay");
	pover->setZOrder(5);

	// screen sized container for game overlay
	pcont = static_cast<Ogre::OverlayContainer*>(omgr->createOverlayElement("Panel", "GUIPanel"));
	pcont->setPosition(0.0f, 0.0f);
	pcont->setMetricsMode(Ogre::GMM_RELATIVE);
	pcont->setVerticalAlignment(Ogre::GVA_TOP);
	pcont->setHorizontalAlignment(Ogre::GHA_LEFT);
	pcont->setDimensions(1.0f, 1.0f);
	pcont->setColour(Ogre::ColourValue(1, 0, 0, 1));
	pover->add2D(pcont);
	pover->show();

	// smaller container in lower left corner for text log
	lcont = static_cast<Ogre::OverlayContainer*>(omgr->createOverlayElement("Panel", "LogPanel"));
	lcont->setPosition(0.0f, 0.0f);
	lcont->setMetricsMode(Ogre::GMM_RELATIVE);
	lcont->setVerticalAlignment(Ogre::GVA_TOP);
	lcont->setHorizontalAlignment(Ogre::GHA_LEFT);
	lcont->setDimensions(0.3f, 0.5f);
	lcont->setColour(Ogre::ColourValue(1, 0, 0, 1));
	pover->add2D(lcont);

	// initialize blank text in log panel
	Ogre::TextAreaOverlayElement * ltext = static_cast<Ogre::TextAreaOverlayElement*>(omgr->createOverlayElement("TextArea", "LogText"));
	ltext->setCaption("");
	ltext->setPosition(0.0, 0.0);
	ltext->setDimensions(0.5f, 0.03f);
	ltext->setCharHeight(0.03f);
	ltext->setFontName("MyFont");
	ltext->setColourBottom(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
	ltext->setColourTop(Ogre::ColourValue(0.5f, 0.5f, 1.0f));
	lcont->addChild(ltext);

	pover->show();

	//initialize counters
	textCount = 0;
	imageCount = 0;
}

ssurge::GUIManager::~GUIManager()
{

}


int ssurge::GUIManager::createTextOverlay(Ogre::String text, float x, float y, Ogre::String panel)
{
	
	Ogre::TextAreaOverlayElement * ptext = static_cast<Ogre::TextAreaOverlayElement*>(omgr->createOverlayElement("TextArea", "Text" + textCount));
	ptext->setCaption(text);
	ptext->setPosition(x, y);
	ptext->setDimensions(0.5f, 0.03f);
	ptext->setCharHeight(0.03f);
	ptext->setFontName("MyFont");
	ptext->setColourBottom(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
	ptext->setColourTop(Ogre::ColourValue(0.5f, 0.5f, 1.0f));
	static_cast<Ogre::OverlayContainer*>(omgr->getOverlayElement(panel))->addChild(ptext);

	int tmp = textCount;
	textCount++;
	return tmp;
}

int ssurge::GUIManager::createImageOverlay(Ogre::String img, float x, float y)
{
	Ogre::OverlayContainer * pimg = static_cast<Ogre::OverlayContainer*>(omgr->createOverlayElement("Panel", "Image" + imageCount));
	pimg->setPosition(x, y);
	pimg->setDimensions(0.25f, 0.25f);
	pimg->setMaterialName(img);
	pcont->addChild(pimg);

	int tmp = imageCount;
	imageCount++;
	return tmp;
}

void ssurge::GUIManager::removeOverlay(Ogre::String name)
{
	omgr->destroyOverlayElement(name);
}

void ssurge::GUIManager::moveOverlay(Ogre::String name, float x, float y)
{
	pcont->getChild(name)->setPosition(x, y);
}

void ssurge::GUIManager::logText(Ogre::String text)
{
	// emtpy log string
	logString = "";

	// add new text to textLog
	textLog.push_back(text);

	if (textLog.size() > 15)
	{
		textLog.erase(textLog.begin());
	}

	// moving strings in textlog to logString with new lines
	for (unsigned int i = 0; i < textLog.size(); i++)
	{
		logString.append(textLog[i]);
		logString.append("\n");
	}

	// set log text to new string
	lcont->getChild("LogText")->setCaption(logString);
}