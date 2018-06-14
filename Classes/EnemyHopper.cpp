#include "EnemyHopper.h"

#include "WorldScene.h"
#include "Character.h"
#include "TriggerBlockZone.h"

#define HP 10

#define SMALL_SPEED 40.0f
#define LARGE_SPEED 40.0f

#define JUMP_SPEED 400.0f// as same as player

#define HURTING_TIME 0.02f

#define MAX_NUMBER_MOVE_FRAMES 5
#define MAX_NUMBER_DIE_FRAMES 9

#define SCALE_FACTOR 1
#define WIDTH_SCALE_FACTOR 0.4f
#define HEIGHT_SCALE_FACTOR 0.4f

#define DELAY_UNIT_MOVE_FRAMES 0.1f
#define DELAY_UNIT_DIE_FRAMES 0.1f

#define JUMP_DELAY 0.5f


USING_NS_CC;


EnemyHopper::~EnemyHopper()
{
	CC_SAFE_RELEASE(_jumpAnimation);
	CC_SAFE_RELEASE(_dieAnimation);
}

bool EnemyHopper::init()
{
	auto scaleFactor = Director::getInstance()->getContentScaleFactor();

	// Super init
	if (!Enemy::init())
	{
		return false;
	}


	_vecMove = Vec2::ZERO;
	
	
	_hp = HP;
	_maxHP = _hp;

	_jumpSpeed = JUMP_SPEED * 1.0f / scaleFactor;

	_charType = Character::CHARACTER_TYPE::ENEMY_HOPPER;
	_team = 2;			// enemy team

	_direction = Character::DIRECTION::RIGHT;

	_hurtTime = HURTING_TIME;

	_state = STATE::IDLE;

	_numLegacy = 1;


	// Anchor point
	this->setAnchorPoint(Vec2(0.5, 0));

	_duration = JUMP_DELAY;

	return true;
}

void EnemyHopper::initSprites()
{
	// INITIAL SPRITES
	_sprite = Sprite::createWithSpriteFrameName("mon_ani_idle.png");
	_sprite->setScale(SCALE_FACTOR);

	this->addChild(_sprite);

	_sprite->setAnchorPoint(Vec2(0.5, 0));
	_sprite->setNormalizedPosition(Vec2(0.45, -0.6));
	

	// CONTENT SIZE
	auto size = Size(_sprite->getBoundingBox().size.width * WIDTH_SCALE_FACTOR, _sprite->getBoundingBox().size.height * HEIGHT_SCALE_FACTOR);
	this->setContentSize(size);


	// ANIMATIONS
	char str[30];
	Vector<SpriteFrame*> animFrames;
	Animation* animation;

	auto spritecache = SpriteFrameCache::getInstance();

	// Move Animation
	for (int i = 0; i < MAX_NUMBER_MOVE_FRAMES; ++i)
	{
		sprintf(str, "mon_ani_move%d.png", i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}
	animation = Animation::createWithSpriteFrames(animFrames, DELAY_UNIT_MOVE_FRAMES);

	_jumpAnimation = Animate::create(animation);
	CC_SAFE_RETAIN(_jumpAnimation);


	animFrames.clear();


	// Die Animation
	for (int i = 0; i < MAX_NUMBER_DIE_FRAMES; ++i)
	{
		sprintf(str, "mon_ani_die%d.png", i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}
	animation = Animation::createWithSpriteFrames(animFrames, DELAY_UNIT_DIE_FRAMES);

	_dieAnimation = EaseIn::create(Animate::create(animation), 0.4);
	CC_SAFE_RETAIN(_dieAnimation);
}

void EnemyHopper::initPhysics()
{
	// Physics Body
	_body = PhysicsBody::createBox(getBoundingBox().size, PhysicsMaterial(1000.0f, 0.0f, 0.0f));

	_body->setDynamic(true);
	_body->setRotationEnable(false);

	this->setPhysicsBody(_body);

	// Sets bismask(s)
	setColliderBitMask();
}


/* public method for make this character jump
* double jump only*/
void EnemyHopper::jump()
{
	// double jump? not jump anymore | being hurted? can't jump
	if (_state == STATE::DOUBLE_JUMP || _state == STATE::HURTED)
		return;

	if (_state == STATE::JUMP)
		_state = STATE::DOUBLE_JUMP;

	if (_state == STATE::IDLE)
	{
		//_vecMove.x = 0;
		_state = STATE::JUMP;
	}

	

	_vecMove.y = _jumpSpeed;
	_body->setVelocity(_vecMove);

	if (_sprite->getNumberOfRunningActions() > 0)
	{
		_sprite->stopAction(_action);
	}
	_action = _jumpAnimation;
	_sprite->runAction(_action);
}

void EnemyHopper::update(float delta)
{
	_duration -= delta;
	if (_duration <= 0)
	{
		_duration = JUMP_DELAY;
		jump();
	}
	
}

// On contact with Player -> SELF EXPLODES
bool EnemyHopper::onContactBegin(const cocos2d::PhysicsContact &contact)
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


	// Retrieves PhysicsBody of target
	auto targetBody = contact.getShapeA()->getBody();

	if (_body == targetBody)		// if current body is of shape A
	{
		targetBody = contact.getShapeB()->getBody();
	}


	// Checks for contact with Block VERTICALLY, when in jumping state
	if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::BLOCK
		&& contact.getContactData()->normal.x == 0
		&& (_state == STATE::DOUBLE_JUMP || _state == STATE::JUMP)
		&& (_body->getVelocity().y < 0))
	{
		_body->setVelocity(Vec2::ZERO);
		_state = STATE::IDLE;
	}

	return true;
}

void EnemyHopper::onDie()
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

	}, _dieTime, "enemy_hopper_dies");


	// Drops Legacy
	dropLegacy();

	AUDIO::play2d("Ragnasaur.death.ogg");
}