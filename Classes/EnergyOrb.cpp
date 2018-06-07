#include "EnergyOrb.h"

#define NUMBER_FRAMES_ANIMATION 25
#define SPRITE_FRAME_INDEX 6

#define DISAPPEAR_FRAME_START_INDEX 5
#define DISAPPEAR_FRAME_END_INDEX 24

#define NORMAL_FRAME_START_INDEX 6
#define NORMAL_FRAME_END_INDEX 12

#define DELAY_UNIT_TIME_DISAPPEAR 0.05f
#define DELAY_UNIT_TIME_NORMAL 0.1f
#define SCALE_FACTOR 0.15f

USING_NS_CC;


EnergyOrb::~EnergyOrb()
{
	unscheduleAllCallbacks();

	//CC_SAFE_DELETE(_normalAction);
	//CC_SAFE_DELETE(_disappearAction);

	CC_SAFE_RELEASE_NULL(_normalAction);
	CC_SAFE_RELEASE_NULL(_disappearAction);
}

bool EnergyOrb::init()
{
	if (!Item::init())
	{
		return false;
	}

	_itemType = Item::ITEM_TYPE::ENERGY_ORB;

	_isDisappearing = false;

	_value = 1;		// Note: energy level is measured in terms of number of orbs

	return true;
}

void EnergyOrb::initSprites()
{
	char str[20];

	// Inits sprite
	sprintf(str, "heal_orb%d.png", SPRITE_FRAME_INDEX);
	_sprite = Sprite::createWithSpriteFrameName(str);

	_sprite->setScale(SCALE_FACTOR);
	_sprite->setNormalizedPosition(Vec2(0.5, 0.5));		// center of the node

	this->addChild(_sprite);


	// Inits animation
	auto spritecache = SpriteFrameCache::getInstance();
	Vector<SpriteFrame*> animFrames;
	Animation* animation;

	// Normal animation
	for (int i = NORMAL_FRAME_START_INDEX; i < NORMAL_FRAME_END_INDEX; ++i)
	{
		sprintf(str, "heal_orb%d.png", i);

		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}
	animation = Animation::createWithSpriteFrames(animFrames, DELAY_UNIT_TIME_NORMAL);

	_normalAction = RepeatForever::create(Animate::create(animation));
	CC_SAFE_RETAIN(_normalAction);


	animFrames.clear();


	// Disappear animation
	for (int i = DISAPPEAR_FRAME_START_INDEX; i <= DISAPPEAR_FRAME_END_INDEX; ++i)
	{
		sprintf(str, "heal_orb%d.png", i);

		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}
	animation = Animation::createWithSpriteFrames(animFrames, DELAY_UNIT_TIME_DISAPPEAR);

	_disappearAction = Animate::create(animation);
	CC_SAFE_RETAIN(_disappearAction);


	// Runs the animation
	_sprite->runAction(_normalAction);
}

void EnergyOrb::initPhysics()
{
	_body = PhysicsBody::createCircle(_sprite->getBoundingBox().size.width * 0.45f, PhysicsMaterial(0.5f, 0.0f, 0.1f));

	_body->setDynamic(false);

	this->addComponent(_body);

	// Set bitmask(s)
	setColliderBitMask();
}

void EnergyOrb::disappear()
{
	// Executes animation
	_sprite->stopAction(_normalAction);
	_sprite->runAction(_disappearAction);

	// Sets disappear state
	_isDisappearing = true;

	// Move to...

	// PoolManager management
	scheduleOnce([&](float delta) {

		setActive(false);

	}, _disappearAction->getDuration(), "set_inactive_scheduler");
	
}

void EnergyOrb::reset(float params[10])
{
	// Super
	Item::reset(params);


	// Re-runs (initial) action
	_sprite->stopAllActions();
	_sprite->runAction(_normalAction);

	_isDisappearing = false;
}
