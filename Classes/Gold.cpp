#include "Gold.h"

#include "Item.h"

#define MAX_NUMBER_ANIMATION_FRAMES 18

#define SCALE_FACTOR 0.5f

USING_NS_CC;

Gold::~Gold()
{
	// Releases animation
	CC_SAFE_RELEASE_NULL(_action);
}

bool Gold::init()
{
	if (!Item::init())
	{
		return false;
	}

	_itemType = Item::ITEM_TYPE::GOLD;

	_value = 10;		// for money

	return true;
}

void Gold::initSprites()
{
	//_sprite = Sprite::createWithSpriteFrameName("coin0.png");
	_sprite = Sprite::createWithSpriteFrameName("gold0.png");

	_sprite->setNormalizedPosition(Vec2(0.5, 0.5));		// center of the node
	_sprite->setScale(SCALE_FACTOR);

	this->addChild(_sprite);

	// [Also] Initializes animation for Gold
	auto spritecache = SpriteFrameCache::getInstance();
	Vector<SpriteFrame*> animFrames;
	char str[20];
	for (int i = 0; i < MAX_NUMBER_ANIMATION_FRAMES; ++i)
	{
		//sprintf(str, "coin%d.png", i);
		sprintf(str, "gold%d.png", i);

		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}

	Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.15f);

	_action = RepeatForever::create(Animate::create(animation));
	CC_SAFE_RETAIN(_action);

	// Runs animation
	_sprite->runAction(_action);
}

void Gold::initPhysics()
{
	_body = PhysicsBody::createCircle(_sprite->getBoundingBox().size.width * 0.45f, PhysicsMaterial(0.1f, 0.0f, 0.5f));

	_body->setDynamic(false);

	this->addComponent(_body);

	// Helper functions for Physics handling
	setColliderBitMask();
}

void Gold::reset(float params[10])
{
	// Super
	Item::reset(params);


	// Re-runs animation
	_sprite->stopAllActions();
	_sprite->runAction(_action);
}

Sprite* Gold::makeSprite()
{
	auto sprite = Sprite::createWithSpriteFrameName("gold0.png");

	sprite->setScale(SCALE_FACTOR);

	return sprite;
}
