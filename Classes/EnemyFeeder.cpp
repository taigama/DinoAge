#include "EnemyFeeder.h"

#include "WorldScene.h"
#include "Character.h"
#include "TriggerBlockZone.h"

#define SMALL_HP 5
#define LARGE_HP 15

#define SMALL_SPEED 40.0f
#define LARGE_SPEED 40.0f

#define JUMP_SPEED 0.0f

#define HURTING_TIME 0.02f

#define MAX_NUMBER_MOVE_FRAMES 5
#define MAX_NUMBER_DIE_FRAMES 9

#define SMALL_SCALE_FACTOR 1.3f
#define LARGE_SCALE_FACTOR 2.6f
#define WIDTH_SCALE_FACTOR 0.4f
#define HEIGHT_SCALE_FACTOR 0.4f

#define DELAY_UNIT_MOVE_FRAMES 0.1f
#define DELAY_UNIT_DIE_FRAMES 0.1f

USING_NS_CC;


EnemyFeeder::~EnemyFeeder()
{
	// Releases animations
	CC_SAFE_RELEASE_NULL(_moveAnimation);
	CC_SAFE_RELEASE_NULL(_dieAnimation);

	// Releases projectiles
	CC_SAFE_RELEASE_NULL(_selfExplode);
}

bool EnemyFeeder::init()
{
	auto scaleFactor = Director::getInstance()->getContentScaleFactor();

	// Super init
	if (!Enemy::init())
	{
		return false;
	}

	
	// Properties
	_feederType = FEEDER_TYPE::STATIONARY;		// default: STATIONARY
	_feederSize = FEEDER_SIZE::SMALL;			// default: SMALL

	_hp = SMALL_HP;
	_maxHP = _hp;

	_speed = SMALL_SPEED * 1.0f / scaleFactor;
	_jumpSpeed = JUMP_SPEED * 1.0f / scaleFactor;

	_charType = Character::CHARACTER_TYPE::ENEMY_FEEDER;
	_team = 2;			// enemy team

	_direction = Character::DIRECTION::LEFT;

	_hurtTime = HURTING_TIME;

	_state = STATE::IDLE;
	_isExploding = false;
	_blockCollision = false;

	_numLegacy = 1;


	// Anchor point
	this->setAnchorPoint(Vec2(0.5, 0));


	return true;
}

void EnemyFeeder::initSprites()
{
	// INITIAL SPRITES
	_sprite = Sprite::createWithSpriteFrameName("mon_ani_idle.png");
	_sprite->setScale(SMALL_SCALE_FACTOR);

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

	_moveAnimation = RepeatForever::create(Animate::create(animation));
	CC_SAFE_RETAIN(_moveAnimation);


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

void EnemyFeeder::initPhysics()
{
	// Physics Body
	_body = PhysicsBody::createBox(getBoundingBox().size, PhysicsMaterial(1000.0f, 0.0f, 0.0f));

	_body->setDynamic(true);
	_body->setRotationEnable(false);

	this->setPhysicsBody(_body);

	// Sets bismask(s)
	setColliderBitMask();
}

void EnemyFeeder::initProjectiles()
{
	_selfExplode = dynamic_cast<SelfExplode*>(Projectile::createProjectile(Projectile::PROJECTILE_TYPE::SELF_EXPLODE, this));

	_selfExplode->setExplosionType((int)_feederSize);
	_selfExplode->setPhysics();

	_selfExplode->setAnchorPoint(Vec2(0.5, 0));
	_selfExplode->setName("self_explosion");
	_selfExplode->setNormalizedPosition(Vec2(0.5, 0));
	
	CC_SAFE_RETAIN(_selfExplode);

	// Sets dying time
	_dieTime = _dieAnimation->getDuration() * 0.8f;
}

void EnemyFeeder::setEnemyFeederType(EnemyFeeder::FEEDER_TYPE type)
{
	CCASSERT(type != EnemyFeeder::FEEDER_TYPE::NONE, "EnemyFeeder type should not be NONE");

	_feederType = type;
}

void EnemyFeeder::setEnemyFeederSize(EnemyFeeder::FEEDER_SIZE size)
{
	CCASSERT(size != EnemyFeeder::FEEDER_SIZE::NONE, "EnemyFeeder size should not be NONE");

	_feederSize = size;

	if (_feederSize == FEEDER_SIZE::LARGE)
	{
		// Re-sets sprite scale
		_sprite->setScale(LARGE_SCALE_FACTOR);
		_sprite->setNormalizedPosition(Vec2(0.45, -0.6));

		// Re-sets content size
		auto size = Size(_sprite->getBoundingBox().size.width * WIDTH_SCALE_FACTOR, _sprite->getBoundingBox().size.height * HEIGHT_SCALE_FACTOR);
		this->setContentSize(size);

		// Re-sets physics body
		this->removeComponent(_body);		// removes current physics body
		initPhysics();						// re-initializes a new one

		_hp = LARGE_HP;
		_maxHP = _hp;

		_speed = LARGE_SPEED;


		// Re-sets Legacy (Gold) dropped when EnemyFeeder dies
		_numLegacy = 2;
	}


	// Initializes projectile(s)
	initProjectiles();
}

void EnemyFeeder::setInitialDirection(DIRECTION direction)
{
	CCASSERT(direction != Character::DIRECTION::NONE, "Direction should not be NONE");

	_direction = direction;
	setScaleX(-(int)_direction);
}

void EnemyFeeder::move(Character::DIRECTION direction)
{
	// Stops current action first (if any)
	if (_sprite->getNumberOfRunningActions() > 0)
	{
		_sprite->stopAction(_action);
	}

	if (direction == DIRECTION::NONE)
	{
		_sprite->initWithSpriteFrameName("mon_ani_idle.png");
		_sprite->setAnchorPoint(Vec2(0.5, 0));
	}
	else if (direction == DIRECTION::LEFT || direction == DIRECTION::RIGHT)
	{
		CCASSERT(direction == _direction, "Argument direction must match current direction");

		// Runs animation for walking
		_action = _moveAnimation;
		_sprite->runAction(_action);
	}

	// Calls super
	Enemy::move(direction);
}

void EnemyFeeder::onDie()
{
	//_body->setContactTestBitmask(0);

	// Sets exploding state
	setExplodingState(true);

	// Stops
	move(DIRECTION::NONE);

	// Animation
	if (_sprite->getNumberOfRunningActions() > 0)
	{
		_sprite->stopAction(_action);
	}
	_action = _dieAnimation;
	_sprite->runAction(_action);

	// Sets this object into in-active state
	scheduleOnce([&](float delta) {

		// Notifies the parent Zone that the current Enemy dies
		if (getZone())
		{
			dynamic_cast<TriggerBlockZone*>(this->getZone())->notifyCreepDie(this);
		}

		this->setActive(false);

	}, _dieTime, "enemy_dies");

	_body->setCategoryBitmask(0);
	_body->setContactTestBitmask(0);

	// Drops Legacy
	dropLegacy();

	AUDIO::play2d("Ragnasaur.death.ogg");
}

void EnemyFeeder::explode()
{
	// Adds projectile
	scheduleOnce([&](float delta) {

		if (this->getChildByName("self_explosion") == nullptr)
		{
			this->addChild(_selfExplode);
		}
		else
		{
			CCLOG("explode(): SelfExplode projectile had been already added");
		}

	}, _dieTime * 0.3f, "explode_scheduler");
	
	// Schedules to remove projectile
	scheduleOnce([&](float delta) {

		this->removeChildByName("self_explosion");

	}, _dieTime * 0.5f, "remove_projectile_scheduler");
}

void EnemyFeeder::startAction()
{
	if (_feederType == FEEDER_TYPE::MOVING)
	{
		move(_direction);
	}
}

void EnemyFeeder::update(float delta)
{
	//// IF explodes
	//if (_isExploding)
	//{
	//	explode();
	//	this->setExplodingState(false);		// avoids calling explode() multiple time
	//	
	//	return;
	//}


	// IF collides with Block (horizontally)
	if (_blockCollision)
	{
		// Stops -> then turns around
		move(DIRECTION::NONE);

		turnAround();

		// Moves forward the currenly facing direction
		move(_direction);

		// Resets state
		setBlockCollision(false);
	}
}

// On contact with Player -> SELF EXPLODES
bool EnemyFeeder::onContactBegin(const cocos2d::PhysicsContact &contact)
{
	if (_isExploding)
		return false;

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


	// Checks for contact with Player
	if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::CHARACTER)
	{
		auto character = dynamic_cast<Character*>(targetBody->getOwner());

		// NOT same team
		if (character->getTeam() != _team)
		{
			//onDie();			// EnemyFeeder dies -> SELF EXPLODES
			// no longer die. Now, it's stopped
			setBlockCollision(true);
		}
	}

	// Checks for contact with Block HORIZONTALLY
	if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::BLOCK && contact.getContactData()->normal.y == 0)
	{
		// Turns on collision flag
		setBlockCollision(true);
	}

	return true;
}

void EnemyFeeder::onEnable()
{
	_body->setCategoryBitmask((int)OBJECT_TYPE::CHARACTER);	// character

	_body->setCollisionBitmask(
		(int)OBJECT_TYPE::CHARACTER
		| (int)OBJECT_TYPE::BLOCK
	);
	_body->setContactTestBitmask(
		(int)OBJECT_TYPE::CHARACTER
		| (int)OBJECT_TYPE::PROJECTILE
		| (int)OBJECT_TYPE::BLOCK
		| (int)OBJECT_TYPE::ITEM
		| (int)OBJECT_TYPE::ZONE
	);

	setBlockCollision(false);
	setExplodingState(false);
	this->scheduleUpdate();
}