#include "SpeechZone.h"


#include "WorldScene.h"
#include "PlayScene.h"
#include "Speecher.h"

#include "ResourceManager.h"

// the time for playing animation, before the screen be faded out
#define TIME_REMAIN 1.0f

USING_NS_CC;

bool SpeechZone::init()
{
	if (!Zone::init())
		return false;
	m_fileName = "";
	// pre-allocate memory
	m_fileName.reserve(16);

	return true;
}


// when player enter the zone
void SpeechZone::onPlayerEntered(Character* target)
{
	if (m_fileName == "")
	{
		MessageBox("File name was empty!", "SpeechZone::message");
		return;
	}

	PlayScene* playScene = (PlayScene*)World::getCurrent()->getParent();
	playScene->getSpeecher()->activeSpeech(m_fileName);
}

void SpeechZone::message(std::string your_message)
{
	if (your_message == "")
	{
		MessageBox("File name was empty!", "SpeechZone::message");
		return;
	}

	m_fileName = your_message;

	ResourceManager::getInstance()->loadSpeechDatas(m_fileName);
}