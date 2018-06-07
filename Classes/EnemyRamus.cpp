#include "EnemyRamus.h"

#define HP 20.0f
#define SPEED 250.0f
#define JUMP_SPEED 0.0f

#define HURTING_TIME 0.2f

#define IDLE_FRAMES_START_INDEX 0
#define IDLE_FRAMES_END_INDEX 3
#define DELAY_UNIT_TIME_IDLE 0.2f
#define IDLE_DURATION 1.0f

#define PREPARE_FRAMES_START_INDEX 4
#define PREPARE_FRAMES_END_INDEX 20
#define DELAY_UNIT_TIME_PREPARE 0.05f

#define ROLLING_FRAMES_START_INDEX 21
#define ROLLING_FRAMES_END_INDEX 43
#define DELAY_UNIT_TIME_ROLL 0.1f
#define TIME_ROLLING_AT_PLACE 0.3f

#define RETURN_IDLE_FRAMES_START_INDEX 44
#define RETURN_IDLE_FRAMES_END_INDEX 55
#define DELAY_UNIT_TIME_RETURN_IDLE 0.15f

#define SCALE_X_LEFT +1
#define SCALE_X_RIGHT -1

#define WIDTH_SCALE_FACTOR 0.36f
#define HEIGHT_SCALE_FACTOR 0.38f
#define SPRITE_SCALE_FACTOR 0.8f

#define PROJECTILE_ROLL_WIDTH_FACTOR 1.1f
#define PROJECTILE_ROLL_HEIGHT_FACTOR 1.1f

#define SAFE_OFFSET 10.0f

USING_NS_CC;


EnemyRamus::~EnemyRamus()
{
	// Releases animations
	CC_SAFE_RELEASE_NULL(_idleAnimation);
	CC_SAFE_RELEASE_NULL(_returnIdleAnimation);
	CC_SAFE_RELEASE_NULL(_prepareAnimation);
	CC_SAFE_RELEASE_NULL(_rollingAnimation);

	// Releases projectile(s)
	CC_SAFE_RELEASE_NULL(_rollProjectile);
}

bool EnemyRamus::init()
{
	// ------------------------------
	// REMEBER TO SET TWO ENDPOINTS
	// ------------------------------

	auto scaleFactor = Director::getInstance()->getContentScaleFactor();

	// Super init()
	if (!Enemy::init())
	{
		return false;
	}

	// Initializes properties of Character
	_hp = HP;
	_maxHP = HP;

	_speed = SPEED * 1.0f / scaleFactor;				// actually, this is the attack speed of EnemyRamus
	_jumpSpeed = JUMP_SPEED * 1.0f / scaleFactor;

	_charType = Character::CHARACTER_TYPE::ENEMY_RAMUS;
	_team = 2;											// enemy team

	_direction = Character::DIRECTION::LEFT;			// temporary direction -> determine actual one after setting two endpoints for "rolling"

	_hurtTime = HURTING_TIME;

	_state = STATE::IDLE;
	_hasTurn = false;

	_numLegacy = 1;

	// Initializes Projectile(s) for EnemyRamus
	initProjectiles();


	this->setAnchorPoint(Vec2(0.5, 0));


	return true;
}

void EnemyRamus::initSprites()
{
	// --------------------------------- INITIAL SPRITE -------------------------------------

	// Load texture for sprite
	_sprite = Sprite::createWithSpriteFrameName("ramus0.png");
	_sprite->setScale(SPRITE_SCALE_FACTOR);

	this->addChild(_sprite);

	// Sets content size of the Node based on its sprite
	auto size = Size(_sprite->getContentSize().width * WIDTH_SCALE_FACTOR, _sprite->getContentSize().height * HEIGHT_SCALE_FACTOR);
	this->setContentSize(size);

	// Sets position relative to node
	_sprite->setAnchorPoint(Vec2(0.5, 0));
	_sprite->setNormalizedPosition(Vec2(0.5, -0.15));

	// --------------------------------------------------------------------------------------


	// ---------------------------------- ANIMATIONS --------------------------------------
	
	// Load animations
	char str[20];
	auto spritecache = SpriteFrameCache::getInstance();
	Vector<SpriteFrame*> animFrames;
	Animation* animation;

	// Idle animation(s)
	for (int i = IDLE_FRAMES_START_INDEX; i <= IDLE_FRAMES_END_INDEX; ++i)		// (normal) idle
	{
		sprintf(str, "ramus%d.png", i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}
	animation = Animation::createWithSpriteFrames(animFrames, DELAY_UNIT_TIME_IDLE);

	_idleAnimation = Animate::create(animation);
	CC_SAFE_RETAIN(_idleAnimation);


	animFrames.clear();


	for (int i = RETURN_IDLE_FRAMES_START_INDEX; i <= RETURN_IDLE_FRAMES_END_INDEX; ++i)	// return to idle state (after rolling)
	{
		sprintf(str, "ramus%d.png", i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}
	animation = Animation::createWithSpriteFrames(animFrames, DELAY_UNIT_TIME_RETURN_IDLE);

	_returnIdleAnimation = Animate::create(animation);
	CC_SAFE_RETAIN(_returnIdleAnimation);


	animFrames.clear();


	// Attack animation(s)
	for (int i = PREPARE_FRAMES_START_INDEX; i <= PREPARE_FRAMES_END_INDEX; ++i)	// prepare (charge) before rolling
	{
		sprintf(str, "ramus%d.png", i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}
	animation = Animation::createWithSpriteFrames(animFrames, DELAY_UNIT_TIME_PREPARE);

	_prepareAnimation = Animate::create(animation);
	CC_SAFE_RETAIN(_prepareAnimation);


	animFrames.clear();


	for (int i = ROLLING_FRAMES_START_INDEX; i <= ROLLING_FRAMES_END_INDEX; ++i)	// rolling (a.k.a. attacking)
	{
		sprintf(str, "ramus%d.png", i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}
	animation = Animation::createWithSpriteFrames(animFrames, DELAY_UNIT_TIME_ROLL);

	_rollingAnimation = Animate::create(animation);
	CC_SAFE_RETAIN(_rollingAnimation);

	// ----------------------------------------------------------------------------------
}

void EnemyRamus::initPhysics()
{
	// Physics Body
	_body = PhysicsBody::createBox(getBoundingBox().size, PhysicsMaterial(1000.0f, 0.0f, 0.0f));

	_body->setDynamic(true);
	_body->setRotationEnable(false);	// avoid rotation when two bodies collide at corner

	this->setPhysicsBody(_body);

	// Sets bitmask(s)
	setColliderBitMask();
}

/**
 * Initializes RollAttack as Projectile for EnemyRamus
 *
 * This Projectile is enabled only in rolling stance
 */
void EnemyRamus::initProjectiles()
{
	// Roll Projectile
	_rollProjectile = dynamic_cast<RollAttack*>(Projectile::createProjectile(Projectile::PROJECTILE_TYPE::ROLL_ATTACK, this));

	_rollProjectile->setContentSize(Size(getBoundingBox().size.width * PROJECTILE_ROLL_WIDTH_FACTOR, getBoundingBox().size.height * PROJECTILE_ROLL_HEIGHT_FACTOR));
	_rollProjectile->setAnchorPoint(Vec2(0.5, 0));
	_rollProjectile->setName("roll_projectile");

	_rollProjectile->setPhysics();								// sets PhysicsBody for RollAttack object

	_rollProjectile->setNormalizedPosition(Vec2(0.5, 0));		// relative position to EnemyRamus

	CC_SAFE_RETAIN(_rollProjectile);
}

bool EnemyRamus::setPatrolArea(cocos2d::Vec2 start, cocos2d::Vec2 end)
{
	_start = start;
	_end = end;

	// Determine initial direction
	if (_start.x < _end.x)
	{
		_direction = Character::DIRECTION::RIGHT;
		this->setScaleX(SCALE_X_RIGHT);				// change direction of sprite accordingly
	}
	else
	{
		_direction = Character::DIRECTION::LEFT;
		this->setScaleX(SCALE_X_LEFT);				// change direction of sprite accordingly
	}

	return true;
}

void EnemyRamus::move(Character::DIRECTION direction)
{
	// direction == DIRECTION::NONE -> STOPS moving
	if (direction == Character::DIRECTION::NONE)
	{
		// Sets attacking stance to false
		setAttackingState(false);
	}
	else
	{
		// Passed argument must match the currently facing direction
		CC_ASSERT(direction == _direction);
	}

	// Calls super
	Enemy::move(direction);
}

/**
 * EnemyRamus charges in the currently facing direction, then
 * waits before charging the opposite one; and repeats the loop
 *
 * Order of actions:
 *   + Stand idly -> idle-animation
 *   + Start to "wrap" into attacking stance -> prepare-animation
 *   + Roll at place, then roll forward -> roll-animation
 *   + "Un-wrap" and return to idle stance -> return-idle-animation
 */
void EnemyRamus::attackPhysical()
{
	// Stands idly
	if (_sprite->getNumberOfRunningActions() > 0)
	{
		_sprite->stopAction(_action);
	}
	_action = RepeatForever::create(_idleAnimation);
	_sprite->runAction(_action);


	// Prepares for attack
	scheduleOnce([&](float delta) {

		// "Wrap" into attacking stance
		this->prepareAttack();

	}, IDLE_DURATION, "prepare_attack_scheduler");


	// Attacks (Rolls) forward the current direction
	scheduleOnce([&](float delta) {

		// Roll
		this->rollAttack();

	}, IDLE_DURATION + _prepareAnimation->getDuration(), "attack_scheduler");
}

void EnemyRamus::prepareAttack()
{
	// Executes animation
	if (_sprite->getNumberOfRunningActions() > 0)
	{
		_sprite->stopAction(_action);
	}
	//_action = _prepareAnimation;
	_action = EaseIn::create(_prepareAnimation, 0.3);
	_sprite->runAction(_action);
}

void EnemyRamus::rollAttack()
{
	// Executes animation
	if (_sprite->getNumberOfRunningActions() > 0)
	{
		_sprite->stopAction(_action);
	}
	_action = RepeatForever::create(_rollingAnimation);
	_sprite->runAction(_action);

	//   1. Rolling at place first
	//   2. After that -> run move towards
	scheduleOnce([&](float delta) {

		// Sets state
		setAttackingState(true);

		this->move(this->getDirection());

	}, TIME_ROLLING_AT_PLACE, "roll_at_place_scheduler");

	// Attach RollAttack projectile to this EnemyRamus
	if (this->getChildByName("roll_projectile") == nullptr)
	{
		this->addChild(_rollProjectile);
	}
	else
	{
		CCLOG("rollAttack(): Roll projectile had been already added");
	}

	// Changes the bitmasks -> turns off collision between EnemyRamus and Player when rolling
	_body->setCollisionBitmask(
		(int)OBJECT_TYPE::BLOCK
	);

	AUDIO::play2d("Rhynsdon.moving.ogg");
}

void EnemyRamus::update(float delta)
{
	auto higher = _start.x > _end.x ? _start : _end;
	auto lower = _start.x < _end.x ? _start : _end;
	auto scaleFactor = Director::getInstance()->getContentScaleFactor();

	// Monitors the relative position of EnemyRamus to two endpoints
	if (!_hasTurn && _direction == Character::DIRECTION::RIGHT)		// facing RIGHT
	{
		// Beyond higher endpoint (in x coordinate)
		if (getPosition().x >= higher.x)
		{
			move(DIRECTION::NONE);		// stops moving
			
			returnToIdle();				// returns to idle state

			scheduleOnce([&](float delta) {

				this->turnAround();

				setTurnState(false);

				attackPhysical();		// continues to attack

			}, _action->getDuration(), "roll_back_scheduler");

			_hasTurn = true;			// EnemyRamus has turned around
		}

		// Below lower endpoint (in x coordinate) or within patrol area
		else
		{
			if (_collideRight && isAttack())
			{
				/*unschedule("roll_back_scheduler");
				unschedule("prepare_attack_scheduler");
				unschedule("attack_scheduler");
				unschedule("roll_at_place_scheduler");*/

				move(DIRECTION::NONE);		// stops moving

				returnToIdle();				// returns to idle state

				scheduleOnce([lower, higher, this](float delta) {

					// if within area -> turn around
					if (this->getPosition().x > lower.x && this->getPosition().x < higher.x)
					{
						this->turnAround();
					}

					setTurnState(false);

					attackPhysical();		// continues to attack

				}, _action->getDuration(), "roll_forward_scheduler");

				_hasTurn = true;			// EnemyRamus has turned around

				// Resets collision state
				_collideRight = false;
			}
		}
	}
	else if (!_hasTurn && _direction == Character::DIRECTION::LEFT)	// facing LEFT
	{
		// Below lower endpoint (in x coordinate)
		if (getPosition().x <= lower.x)
		{
			move(DIRECTION::NONE);		// stops moving

			returnToIdle();				// returns to idle state

			scheduleOnce([&](float delta) {

				this->turnAround();

				setTurnState(false);

				attackPhysical();		// continues to attack

			}, _action->getDuration(), "roll_back_scheduler");

			_hasTurn = true;			// EnemyRamus has turned around
		}

		// Beyond higher endpoint (in x coordinate) or within patrol area
		else
		{
			if (_collideLeft && isAttack())
			{
				/*unschedule("roll_back_scheduler");
				unschedule("prepare_attack_scheduler");
				unschedule("attack_scheduler");
				unschedule("roll_at_place_scheduler");*/

				move(DIRECTION::NONE);		// stops moving

				returnToIdle();				// returns to idle state

				scheduleOnce([lower, higher, this](float delta) {

					// if within area -> turn around
					if (this->getPosition().x > lower.x && this->getPosition().x < higher.x)
					{
						this->turnAround();
					}

					setTurnState(false);

					attackPhysical();		// continues to attack

				}, _action->getDuration(), "roll_forward_scheduler");

				_hasTurn = true;			// EnemyRamus has turned around

				// Resets collision state
				_collideLeft = false;
			}
		}
	}
}

void EnemyRamus::returnToIdle()
{
	// Executes animation
	if (_sprite->getNumberOfRunningActions() > 0)
	{
		_sprite->stopAction(_action);
	}
	_action = _returnIdleAnimation;
	_sprite->runAction(_action);

	// Detach RollAttack from this EnemyRamus
	if (this->getChildByName("roll_projectile") != nullptr)
	{
		this->removeChild(_rollProjectile, false);
	}

	// Changes the bitmasks back -> turns on the collision between EnemyRamus and Player again
	_body->setCollisionBitmask(
		(int)OBJECT_TYPE::CHARACTER
		| (int)OBJECT_TYPE::BLOCK
	);
}

void EnemyRamus::onHit(float dmg, DIRECTION dir, float force)
{
	float actualDamage = dmg;

	if (_isAttacking)
	{
		actualDamage /= 2.0f;	// reduces by half
	}

	// Call super
	Enemy::onHit(actualDamage, dir, force);

	// NO animation for hurting (maybe...)
}

void EnemyRamus::onDie()
{
	// Stops currently running actions of EnemyRamus
	move(DIRECTION::NONE);
	if (_sprite->getNumberOfRunningActions() > 0)
	{
		_sprite->stopAction(_action);
	}

	// Stops running schedulers
	unscheduleAllCallbacks();


	// Removes Roll projectile when EnemyRamus dies
	if (this->getChildByName("roll_projectile") != nullptr)
	{
		this->removeChild(_rollProjectile, false);
	}


	// Returns the sprite to initial state
	_sprite->initWithSpriteFrameName("ramus0.png");
	_sprite->setAnchorPoint(Vec2(0.5, 0));


	// Calls super
	Enemy::onDie();
}

bool EnemyRamus::onContactBegin(const cocos2d::PhysicsContact &contact)
{
	// Super
	if (!Enemy::onContactBegin(contact))
	{
		return false;
	}


	// Determines target body
	auto target = contact.getShapeA()->getBody();
	if (target == _body)
	{
		target = contact.getShapeB()->getBody();
	}


	// Collide with STATIC_BLOCK (horizontally)
	if (target->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::BLOCK &&
		contact.getContactData()->normal.y == 0)
	{
		// Collides on the LEFT
		if (target->getPosition().x < _body->getPosition().x)
		{
			_collideLeft = true;
		}

		// Collides on the RIGHT
		else if (target->getPosition().x > _body->getPosition().x)
		{
			_collideRight = true;
		}
	}

	return true;
}

void EnemyRamus::startAction()
{
	attackPhysical();
}
