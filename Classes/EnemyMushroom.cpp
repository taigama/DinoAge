#include "EnemyMushroom.h"

#include "WorldScene.h"
#include "Character.h"
#include "TriggerBlockZone.h"

#define BASE_HP 40

#define HURTING_TIME 0.02f

#define MAX_NUMBER_HITTED_FRAMES 4
#define MAX_NUMBER_DIE_FRAMES 3

#define SCALE_FACTOR 1
#define WIDTH_SCALE_FACTOR 0.4f
#define HEIGHT_SCALE_FACTOR 0.4f

#define DELAY_UNIT_HIT_FRAMES 0.1f
#define DELAY_UNIT_DIE_FRAMES 0.1f

#define BOX_SIZE_WIDTH 30.0f
#define BOX_SIZE_HEIGHT 60.0f


USING_NS_CC;


EnemyMushroom::~EnemyMushroom()
{
	CC_SAFE_RELEASE(_hittedAnimation);
	CC_SAFE_RELEASE(_dieAnimation);
}

bool EnemyMushroom::init()
{
	auto scaleFactor = Director::getInstance()->getContentScaleFactor();

	// Super init
	if (!Enemy::init())
	{
		return false;
	}

	_scaleMushroom = 1.0f;

	
	_hp = BASE_HP;
	_maxHP = _hp;


	_charType = Character::CHARACTER_TYPE::ENEMY_MUSHROOM;
	_team = 2;			// enemy team

	_direction = Character::DIRECTION::LEFT;

	_hurtTime = HURTING_TIME;

	_state = STATE::IDLE;

	_numLegacy = 1;

	// Sets dying time
	_dieTime = _dieAnimation->getDuration() * 0.8f;

	// Anchor point
	this->setAnchorPoint(Vec2(0.5, 0));


	return true;
}

void EnemyMushroom::initSprites()
{
	// INITIAL SPRITES
	_sprite = Sprite::createWithSpriteFrameName("mushroom_stand.png");
	_sprite->setScale(SCALE_FACTOR);

	this->addChild(_sprite);

	_sprite->setAnchorPoint(Vec2(0.5, 0));
	_sprite->setNormalizedPosition(Vec2(0.5, -0.6));
	

	// CONTENT SIZE
	auto size = Size(_sprite->getBoundingBox().size.width * WIDTH_SCALE_FACTOR, _sprite->getBoundingBox().size.height * HEIGHT_SCALE_FACTOR);
	this->setContentSize(size);


	// ANIMATIONS
	char str[30];
	Vector<SpriteFrame*> animFrames;
	Animation* animation;

	auto spritecache = SpriteFrameCache::getInstance();

	// Move Animation
	for (int i = 0; i < MAX_NUMBER_HITTED_FRAMES; i++)
	{
		sprintf(str, "mushroom_hitted%d.png", i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}
	animation = Animation::createWithSpriteFrames(animFrames, DELAY_UNIT_HIT_FRAMES);

	_hittedAnimation = Animate::create(animation);
	CC_SAFE_RETAIN(_hittedAnimation);


	animFrames.clear();


	// Die Animation
	for (int i = 0; i < MAX_NUMBER_DIE_FRAMES; i++)
	{
		sprintf(str, "mushroom_die%d.png", i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}
	animation = Animation::createWithSpriteFrames(animFrames, DELAY_UNIT_DIE_FRAMES);

	_dieAnimation = EaseIn::create(Animate::create(animation), 0.4);
	CC_SAFE_RETAIN(_dieAnimation);
}

void EnemyMushroom::initPhysics()
{
	// Physics Body
	_body = PhysicsBody::createBox(Size(BOX_SIZE_WIDTH, BOX_SIZE_HEIGHT), PhysicsMaterial(1000.0f, 0.0f, 0.0f));

	_body->setDynamic(true);
	_body->setRotationEnable(false);

	this->setPhysicsBody(_body);

	// Sets bismask(s)
	setColliderBitMask();
}

// On contact with Player -> SELF EXPLODES
bool EnemyMushroom::onContactBegin(const cocos2d::PhysicsContact &contact)
{
	if (_isExploding)
	{
		return false;
	}

	// Super
	if (!Enemy::onContactBegin(contact))
	{
		return false;
	}
	return true;
}

void EnemyMushroom::onHit(float dmg, DIRECTION dir, float force)
{
	// Animation
	if (_sprite->getNumberOfRunningActions() > 0)
	{
		_sprite->stopAction(_action);
	}
	_action = _hittedAnimation;
	_sprite->runAction(_action);


	// Calls super version
	Character::onHit(dmg, dir, force);

	if (dir != DIRECTION::LEFT && dir != DIRECTION::RIGHT)
		return;

	this->_direction = (DIRECTION)(-1 * (int)dir);
	this->setScaleX(((int) _direction) * _scaleMushroom);
}

void EnemyMushroom::onDie()
{
	//_body->setContactTestBitmask(0);

	// Sets exploding state
	setExplodingState(true);

	// Animation
	if (_sprite->getNumberOfRunningActions() > 0)
	{
		_sprite->stopAction(_action);
	}
	_action = _dieAnimation;
	_sprite->runAction(_action);

	// Sets this object into in-active state
	scheduleOnce([&](float delta) {
		
		this->setActive(false);

	}, _dieTime, "enemy_mushroom_dies");


	// Drops Legacy
	dropLegacy();

	AUDIO::play2d("Ragnasaur.death.ogg");
}

void EnemyMushroom::setScaleMushroom(float scale)
{
	_scaleMushroom = scale;

	this->setScaleX(this->getScaleX() * _scaleMushroom);
	this->setScaleY(this->getScaleY() * _scaleMushroom);

	_maxHP = BASE_HP * _scaleMushroom;
	_hp = _maxHP;
}