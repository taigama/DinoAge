#include "NextStageZone.h"

//#include "MyPoolManager.h"
//#include "WorldScene.h"
//#include "ResourceManager.h"
#include "PlayScene.h"

// the time for playing animation, before the screen be faded out
#define TIME_REMAIN 1.0f

USING_NS_CC;

bool NextStageZone::init()
{
	if (!Zone::init())
		return false;
	m_mapName = "";
	// pre-allocate memory
	m_mapName.reserve(16);

	return true;
}

void NextStageZone::initSprites()
{
	_sprite = Sprite::create();
	this->addChild(_sprite);

	SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();
	Vector<SpriteFrame*> frames;// list of frames
	char str[100] = { 0 };// string
	SpriteFrame* frame;// tmp frame


	for (int i = 1; i <= 18; i++)
	{
		sprintf(str, "aura_fairy/aura_fairy (%d).png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	auto animation = Animation::createWithSpriteFrames(frames, 0.1f);
	auto repeater = RepeatForever::create(Animate::create(animation));
	frames.clear();

	_sprite->runAction(repeater);
}

// when player enter the zone
void NextStageZone::onPlayerEntered(Character* target)
{
	if (m_mapName == "")
	{
		MessageBox("Map name was empty!", "NextStageZone::message");
		return;
	}

	// schedule something
	
	
	scheduleOnce(
		[this](float delay) {

		PlayScene* playScene = (PlayScene*) this->getParent()->getParent();
		playScene->changeMap(this->m_mapName, this->_levelTitle, this->_stageTitle);

		// remove this zone (no need anymore)
		this->setActive(false);

	}, 0.0f, "Change_World");
}

void NextStageZone::message(std::string your_message)
{
	if (your_message == "")
	{
		MessageBox("Map name was empty!", "NextStageZone::message");
		return;
	}

	m_mapName = your_message;
}