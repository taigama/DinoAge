#include "Teleporter.h"

#include "MyPoolManager.h"
#include "WorldScene.h"
#include "ResourceManager.h"

#define TIME_TELEPORT 1.0f

USING_NS_CC;


Teleporter::~Teleporter() {}

bool Teleporter::init()
{
	if (!Zone::init())
		return false;

	m_targetLocation = Vec2::ZERO;
	m_target = nullptr;

	m_timer = TIME_TELEPORT;

	return true;
}

void Teleporter::initSprites()
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
void Teleporter::onPlayerEntered(Character* target)
{
	m_target = target;
}

void Teleporter::update(float delta)
{
	if (m_target)
	{
		if (m_target->getState() == Character::STATE::IDLE)
		{
			m_timer -= delta;

			// ----- test ------
			char str[100] = { 0 };
			sprintf(str, "timer: %.2f", m_timer);
			Label* label = (Label*)m_target->getChildByTag(123123);
			if (label == nullptr)
			{
				label = Label::createWithTTF(str, "fonts/Marker Felt.ttf", 15);
				label->setPosition(Vec2(0, 60));
				m_target->addChild(label, 5, 123123);
			}
			else
			{
				label->setString(str);
			}
			// ------------------------------

			// time up! move player to the new location
			if (m_timer <= 0)
			{
				// move position
				m_target->setPosition(m_targetLocation);
				m_target = nullptr;
				m_timer = TIME_TELEPORT;

				// add effect
				SPAWN_EFFECT(Effect::EFFECT_TYPE::FLASH_BLUE// type fx
					, this->getPosition().x	, this->getPosition().y // position
					, 1 // direction (no need)
					, 1 // repeat times
					, 1 // scale
					, World::WORLD_LAYER::EFFECT_FRONT); // layer in world

				SPAWN_EFFECT(Effect::EFFECT_TYPE::FLASH_BLUE// type fx
					, m_targetLocation.x, m_targetLocation.y // position
					, 1 // direction (no need)
					, 1 // repeat times
					, 1 // scale
					, World::WORLD_LAYER::EFFECT_FRONT); // layer in world
			}


			
		}
		else
		{
			m_timer = TIME_TELEPORT;
		}
	}
}

void Teleporter::onPlayerExit(Character* target)
{
	// m_target = null? <lol> that will be a problem if target == null
	if (target == m_target)
	{
		// ----- test ------
		char str[100] = { 0 };
		sprintf(str, "exit!");
		Label* label = (Label*)m_target->getChildByTag(123123);
		if (label == nullptr)
		{
			label = Label::createWithTTF(str, "fonts/Marker Felt.ttf", 15);
			label->setPosition(Vec2(0, 60));
			m_target->addChild(label, 5, 123123);
		}
		else
		{
			label->setString(str);
		}
		// ------------------------------

		m_target = nullptr;
	}
}

void Teleporter::init(float x1, float y1
	, int start_index, int end_index
	, int i_value, int ID, int direction)
{
	float heightMap = ResourceManager::getInstance()->getArea().size.height;

	m_targetLocation.x = x1;
	m_targetLocation.y = heightMap - y1;

	_ID = ID;
}