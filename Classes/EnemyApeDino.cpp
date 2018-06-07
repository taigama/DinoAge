#include "EnemyApeDino.h"

#include "RedDino.h"
#include "Spike.h"
#include "Charge.h"
#include "Projectile.h"
#include "WorldScene.h"
#include "Block.h"

#define WIDTH_FACTOR 0.62f
#define HEIGHT_FACTOR 0.70f
#define SCALE_STATUS_SPRITES 0.1f

#define SPIKE_WIDTH_SCALE_FACTOR 0.70f

#define DELAY_FRAME_WALK 0.20f
#define DELAY_FRAME_ATTACK 0.10f
#define DELAY_TIME_FOR_CHARGE 1.5f
#define DELAY_TIME_FOR_LOOKING_PLAYER 1.5f

#define HP 50.0f
#define SPEED 25.0f
#define ATTACK_SPEED 280.0f
#define JUMP_SPEED 0.0f
#define DELAY_TIME 1.0f
#define HURTING_TIME 0.2f

#define NUMBER_OF_FRAMES 4

#define TIMES_LOOKING_AROUND 4

#define REPEAT_ACTION_TAG 100

#define CHARGE_COLLIDE_BLOCK_DAMAGE 10.0f
#define REFLECT_KNOCK_BACK_FORCE_FACTOR 0.5f

#define SCALE_X_LEFT +1
#define SCALE_X_RIGHT -1

USING_NS_CC;


EnemyApeDino::~EnemyApeDino()
{
	// Releases projectiles
	CC_SAFE_RELEASE_NULL(m_charge);

	// Releases animations
	CC_SAFE_RELEASE_NULL(m_animation);
	CC_SAFE_RELEASE_NULL(m_action);

}

bool EnemyApeDino::init()
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

	m_charge = nullptr;
	m_start = Vec2(-999, -999);
	m_end = Vec2(-999, -999);
	m_angleRad = 0.0f;
	setMovingFlag(true);

	// Sets values for Character's properties
	_maxHP = HP;
	_hp = HP;

	//_speed = SPEED;			// original walking speed
	_speed = (1.0 / scaleFactor) * SPEED;	// scaled walking speed

	//_attackSpeed = ATTACK_SPEED;		// original attack speed
	_attackSpeed = (1.0 / scaleFactor) * ATTACK_SPEED;	// scaled attack speed

	//_jumpSpeed = JUMP_SPEED;			// original jump speed
	_jumpSpeed = (1.0 / scaleFactor) * JUMP_SPEED;	// scaled jump speed

	_charType = Character::CHARACTER_TYPE::ENEMY_APE_DINO;
	_team = 2;		// enemy team

	// Default direction: to the RIGHT
	_direction = Character::DIRECTION::LEFT;

	// Sets delay time property
	_delayTime = DELAY_TIME;

	// Sets initial state
	_state = Character::STATE::IDLE;
	_knockedBack = false;
	_hitGroundKB = false;
	_isFlying = false;
	_lookingAround = false;

	// Required time for hurting and dying
	_hurtTime = HURTING_TIME;

	// Number of Legacy dropped when dies
	_numLegacy = 2;


	// Initializes sprite and physics body
	//     -> Has already been called on GameObject::init()

	// Projectiles
	initProjectiles();


	this->setAnchorPoint(Vec2(0.5, 0));
	

	return true;
}

void EnemyApeDino::initSprites()
{
	// Loads texture for sprite
	_sprite = Sprite::createWithSpriteFrameName("ape_dino0.png");

	// Sets size of Node based on sprite
	//    - Modifies with m_widthFactor and m_heightFactor
	this->setContentSize(Size(_sprite->getContentSize().width * WIDTH_FACTOR,
		_sprite->getContentSize().height * HEIGHT_FACTOR));

	// Adds to "this"node
	this->addChild(_sprite, 0);

	// Sets position of Sprite in respect to Node
	_sprite->setAnchorPoint(Vec2(0.5, 0));
	_sprite->setNormalizedPosition(Vec2(0.5, 0));

	// Load animations
	auto spritecache = SpriteFrameCache::getInstance();
	Vector<SpriteFrame*> animFrames;

	for (int i = 0; i < NUMBER_OF_FRAMES; ++i)
	{
		char str[14];
		sprintf(str, "ape_dino%d.png", i);

		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}

	m_animation = Animation::createWithSpriteFrames(animFrames, DELAY_FRAME_WALK);
	m_action = RepeatForever::create(Animate::create(m_animation));
	CC_SAFE_RETAIN(m_action);
	CC_SAFE_RETAIN(m_animation);


	// Loads sprites for indicative status
	_questionMark = Sprite::createWithSpriteFrameName("question_mark.png");
	_sprite->addChild(_questionMark);

	_questionMark->setScale(SCALE_STATUS_SPRITES);
	_questionMark->setAnchorPoint(Vec2(0.5, 0));
	_questionMark->setVisible(false);
	_questionMark->setNormalizedPosition(Vec2(0.25, 1.05));

	_exclamationMark = Sprite::createWithSpriteFrameName("exclamation_mark.png");
	_sprite->addChild(_exclamationMark);
	
	_exclamationMark->setScale(SCALE_STATUS_SPRITES);
	_exclamationMark->setAnchorPoint(Vec2(0.5, 0));
	_exclamationMark->setVisible(false);
	_exclamationMark->setNormalizedPosition(Vec2(0.25, 1.05));

}

void EnemyApeDino::initPhysics()
{
	// Creates physics body
	_body = PhysicsBody::createBox(getBoundingBox().size, PhysicsMaterial(1000.0f, 0.0f, 0.0f));

	_body->setDynamic(true);
	_body->setRotationEnable(false);	// avoid rotation when two bodies collide at corner

	// Adds PhysicsBody to the node
	this->setPhysicsBody(_body);

	// Sets bitmask(s)
	setColliderBitMask();
}

/**
 * Updates two types of projectile (attack)
 *     - Spike -> attached to the back of the object
 *     - Charge -> attached to the front of the object
 */
void EnemyApeDino::initProjectiles()
{
	// Charge projectile
	if (m_charge == nullptr)
	{
		m_charge = (Charge*)Projectile::createProjectile(Projectile::PROJECTILE_TYPE::CHARGE_ATTACK, this);
		m_charge->setContentSize(Size(2, getBoundingBox().size.height * 0.8f));
		m_charge->setAnchorPoint(Vec2(1, 0));
		m_charge->setPhysics();
		m_charge->setName("charge");

		CC_SAFE_RETAIN(m_charge);
		m_charge->setNormalizedPosition(Vec2(-0.1, 0));
	}
	

	// Spike Projectile
	auto spike = (Spike*)Projectile::createProjectile(Projectile::PROJECTILE_TYPE::SPIKE_ATTACK, this);
	spike->setContentSize(Size(getBoundingBox().size.width * SPIKE_WIDTH_SCALE_FACTOR, 1));
	spike->setAnchorPoint(Vec2(0, 0));
	spike->setPhysics();
	spike->setName("spike");

	this->addChild(spike);			// attaches to EnemyApeDino

	spike->setNormalizedPosition(Vec2(0.25, 1.1));
}

// No need right now. Determine two  endpoints based on current position
bool EnemyApeDino::setPatrolArea(Vec2 start, Vec2 end)
{
	// Checks if two end points are on the same altitude
	/*if (start.y != end.y)
	{
		log("Warning: Two endpoints are not on the same altitude");
		log("Warning: Settng endpoints unsuccessfully");
		return false;
	}*/

	m_start = start;
	m_end = end;

	// Determine initial direction
	if (m_start.x < m_end.x)
	{
		_direction = Character::DIRECTION::RIGHT;
		this->setScaleX(SCALE_X_RIGHT);		// change direction of sprite accordingly
	}
	else
	{
		_direction = Character::DIRECTION::LEFT;
		this->setScaleX(SCALE_X_LEFT);		// change direction of sprite accordingly
	}

	return true;
}

void EnemyApeDino::move(Character::DIRECTION direct)
{
	// Stops currenly animation (if any)
	if (_sprite->getNumberOfRunningActions() > 0)
	{
		_sprite->stopAction(m_action);
	}

	// If moves LEFT or RIGHT -> runs animation
	if (direct == Character::DIRECTION::LEFT || direct == Character::DIRECTION::RIGHT)
	{
		_sprite->runAction(m_action);
	}

	// Calls super
	Enemy::move(direct);
}

/**
 * This enemy will charge in the direction that it found Player was in
 *		If Player gets hit -> takes damage and is bounced back
 *		If Player escapes:
 *			If this object hits Block instead -> stun for ... seconds
 *		This enemy will then re-scan for Player
 *			If found -> attack again
 *			If not found -> stand looking for Player (maybe ENDLESSLY...)
 */
void EnemyApeDino::attackPhysical()
{
	// Sets to attacking stance
	m_attackStance = true;

	// First -> stop this enemy, momentarily (also, stop current animation) - prepare for CHARGING
	move(DIRECTION::NONE);

	// Changes delay time between each frame in animation
	m_animation->setDelayPerUnit(DELAY_FRAME_ATTACK);	

	CC_SAFE_RELEASE(m_action);		// release previous animation (walking stance)
	m_action = RepeatForever::create(Animate::create(m_animation));	// animation for attacking stance
	CC_SAFE_RETAIN(m_action);

	// Charge attacks
	scheduleOnce(CC_CALLBACK_0(EnemyApeDino::chargeAttack, this), DELAY_TIME_FOR_CHARGE, "charge_attack");
}

/**
 * Charge attack in the direction currently facing
 */
void EnemyApeDino::chargeAttack()
{
	// First, turns off exclamation mark sign
	_exclamationMark->setVisible(false);

	// Adds Charge projectile
	if (this->getChildByName("charge") == nullptr)
	{
		this->addChild(m_charge);
	}
	else
	{
		CCLOG("chargeAttack(): charge Projectile had been already added");
	}

	// If facing LEFT
	if (getScaleX() == SCALE_X_LEFT)
	{
		_direction = Character::DIRECTION::LEFT;

		auto curVelocityY = _body->getVelocity().y;
		_body->setVelocity(Vec2((int)Character::DIRECTION::LEFT * _attackSpeed, curVelocityY));

		// Sets up animation
		_sprite->runAction(m_action);
	}

	// If facing RIGHT
	else if (getScaleX() == SCALE_X_RIGHT)
	{
		_direction = Character::DIRECTION::RIGHT;

		auto curVelocityY = _body->getVelocity().y;
		_body->setVelocity(Vec2((int)Character::DIRECTION::RIGHT * _attackSpeed, curVelocityY));

		// Sets up animation
		_sprite->runAction(m_action);
	}

	// If NONE
	else
	{
		log("Current direction: NONE -> ERROR ??");
	}
}

bool EnemyApeDino::onContactBegin(const cocos2d::PhysicsContact &contact)
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


	// If this enemy is currently in attacking stance
	if (m_attackStance)
	{
		// If collision with Block and in horizontal direction
		if (target->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::BLOCK &&
			contact.getContactData()->normal.y == 0)
		{
			auto block = dynamic_cast<Block*>(target->getOwner());

			// If static Block
			if (block->getBlockType() == (int)Block::BLOCK_TYPE::STATIC)
			{
				m_attackStance = false;

				// Takes recoil damage from Static Block
				onHit(CHARGE_COLLIDE_BLOCK_DAMAGE, DIRECTION::NONE, 0);


				// IF EnemyApeDino still NOT dies
				if (_hp > 0)
				{
					// Stops moving
					move(DIRECTION::NONE);

					// Sets knocked back state to true -> let update() handle knock back action
					setKnockedBack(true);

					// => getHit() function will be called in onContactBegin() when collide with Ground
				}
			}
		}
	}
	else
	{
		// It this enemy is hurted while NOT in attacking stance and NOT in looking-for-player state
		//if (!getLookingAroundState() && _state == STATE::HURTED)
		if (!getLookingAroundState() && target->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::PROJECTILE)
		{
			// After hurting animation -> turn around to find Player
			scheduleOnce([&](float delta) {

				getHit();

			}, _hurtTime, "get_hurt_found_player");
		}

		// If is knocked back and hit the ground
		if (getFlyingState() && target->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::BLOCK &&
			contact.getContactData()->normal.y != 0)
		{
			// Sets state -> let update() handle
			setHitGroundKB(true);
		}
	}

	return true;
}

void EnemyApeDino::patrol()
{
	// Invalidity of two endpoints
	if (m_start == Vec2(-999, -999) && m_end == Vec2(-999, -999))
	{
		CCLOG("ERROR: Two endpoints has not been set");

		return;
	}

	// Sets state
	_state = STATE::RUN;

	// Moves object
	move(_direction);
}

//DrawNode* drawNode;

void EnemyApeDino::update(float delta)
{
	// Draws the ray and contact point(s)
	//if (drawNode)
	//{
	//	Director::getInstance()->getRunningScene()->removeChild(drawNode);
	//}
	//drawNode = DrawNode::create();


	/**
	 * If this object is being knocked back -> handle...
	 */
	if (_knockedBack)
	{
		// First -> stops moving
		move(DIRECTION::NONE);

		// Knock back
		knock(_direction, m_charge->getForce());

		// Sets flying state
		setFlyingState(true);

		setKnockedBack(false);		// reset state

		return;
	}


	/**
	 * If this object hits ground after being knocked back ->....
	 */
	if (_hitGroundKB)
	{
		// [Avoid problem with IceWall skill of WhiteDino]
		// Cancels chrage_attack if hit Static Block horizontally
		unschedule("charge_attack");

		_exclamationMark->setVisible(false);	// turn off exclamation mark sign

		// Stops moving (avoid running backward due to knocked back velocity
		move(DIRECTION::NONE);
		setFlyingState(false);

		setHitGroundKB(false);		// reset state

		getHit();	// instructs object to find Player afterwards

		return;
	}


	// IF is flying (being knocked back) -> skip
	if (getFlyingState())
	{
		return;
	}


	/**
	 * Monitors the eyesight of enemy, using RayCast in PhysicsWorld
	 */

	// Update angle: if beyond limits => re-set value
	if (getScaleX() == SCALE_X_LEFT && m_angleRad < MIN_ANGLE_RAD_EYESIGHT)
	{
		m_angleRad = MIN_ANGLE_RAD_EYESIGHT;
	}
	else if (getScaleX() == SCALE_X_RIGHT && m_angleRad > MIN_ANGLE_RAD_EYESIGHT)
	{
		m_angleRad = MIN_ANGLE_RAD_EYESIGHT;
	}

	// Calculates necessary values to compute starting and ending point of the raycast
	float length = 150.0f / Director::getInstance()->getContentScaleFactor();
	auto distance = Vec2(length * cosf(m_angleRad), length * sinf(m_angleRad));

	// Updates the starting and end points for ray casting
	if (getScaleX() == SCALE_X_LEFT)			// looking to the LEFT
	{
		m_startRay = getPosition() + Vec2(-1 * getContentSize().width * 0.48f, getContentSize().height * 1.18f);
		m_endRay = m_startRay - distance;

		// Determine the increment/decrement to update the angle of eyesight
		if (m_angleRad <= MIN_ANGLE_RAD_EYESIGHT)
		{
			m_factorDeltaAngle = +1;
		}
		else if (m_angleRad >= MAX_ANGLE_RAD_EYESIGHT_LEFT)
		{
			m_factorDeltaAngle = -1;
		}
	}
	else if (getScaleX() == SCALE_X_RIGHT)		// looking to the RIGHT
	{
		m_startRay = getPosition() + Vec2(getContentSize().width * 0.48f, getContentSize().height * 1.18f);
		m_endRay = m_startRay + distance;
			
		// Determine the increment/decrement to update the angle of eyesight
		if (m_angleRad >= MIN_ANGLE_RAD_EYESIGHT)
		{
			m_factorDeltaAngle = -1;
		}
		else if (m_angleRad <= MAX_ANGLE_RAD_EYESIGHT_RIGHT)
		{
			m_factorDeltaAngle = +1;
		}
	}

	// Scans the front area to find Player
	if (!m_noticePlayer)	// not found Player
	{
		Director::getInstance()->getRunningScene()->getPhysicsWorld()->rayCast(
			CC_CALLBACK_3(EnemyApeDino::physicsRayCastFunc, this), m_startRay, m_endRay, nullptr);

		// Draws the casting ray (for debugging)
		//drawNode->drawSegment(m_startRay, m_endRay, 1, Color4F::RED);
		//Director::getInstance()->getRunningScene()->addChild(drawNode);

		// Update angle -> to "rotate" the casting ray
		m_angleRad += 2.5f * m_factorDeltaAngle * (float)M_PI / 180.0f;
	}

	// ----------------------- End of section --------------------------


	/**
	 * Monitors the state of this object while moving
	 *     - If still moving -> do nothing
	 *     - If reaches the endpoint -> stop for _delayTime time; then go back to the other endpoint
	 */
	if (_state == Character::STATE::RUN)
	{
		if (getScaleX() == SCALE_X_LEFT)			// to the LEFT
		{
			float lowerX = m_start.x < m_end.x ? m_start.x : m_end.x;

			// Reaches endpoint
			if (_movingFlag && getPosition().x <= lowerX)
			{
				setMovingFlag(false);					// flips the flag
				move(Character::DIRECTION::NONE);		// stops moving
				this->scheduleOnce([&](float delta) {

					this->move(Character::DIRECTION::RIGHT);	// after waiting -> move in the reversed direction
					setMovingFlag(true);

				}, _delayTime, "move_right");
			}
		}
		else if (getScaleX() == SCALE_X_RIGHT)		// to the RIGHT
		{
			float lowerX = m_start.x > m_end.x ? m_start.x : m_end.x;

			// Reaches endpoint
			if (_movingFlag && getPosition().x >= lowerX)
			{
				setMovingFlag(false);					// flips the flag
				move(Character::DIRECTION::NONE);		// stops moving
				this->scheduleOnce([&](float delta) {

					this->move(Character::DIRECTION::LEFT);		// after waiting -> move in the reversed direction
					setMovingFlag(true);

				}, _delayTime, "move_left");
			}
		}
	}

	// ----------------------- End of section --------------------------

}

bool EnemyApeDino::physicsRayCastFunc(cocos2d::PhysicsWorld& world, const cocos2d::PhysicsRayCastInfo& info, void* data)
{
	// If already found Player -> skip repeating actions again
	if (this->m_noticePlayer)
		return true;


	auto contactBody = info.shape->getBody();
	if (contactBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::CHARACTER)
	{
		// If Character == Player
		auto character = dynamic_cast<Character*>(contactBody->getOwner());
		if (character->getTeam() != _team)		// on opposite team
		{
			m_noticePlayer = true;

			// Stops all running schedulers that may trigger
			unschedule("move_into_patrol_area");
			unschedule("move_right");
			unschedule("move_left");
			unschedule("waiting_re_finding_player");

			this->stopActionByTag(REPEAT_ACTION_TAG);

			// Turns off moving flag
			setMovingFlag(false);

			// Indicates found status -> show exclamation mark sign (and also hide question mark sign, if visible)
			_exclamationMark->setVisible(true);
			_questionMark->setVisible(false);

			// Instructs "this" EnemyApeDino object to attack
			this->attackPhysical();
		}
	}

	return true;
}

void EnemyApeDino::getHit()
{
	// Removes Charge projectile (since this enemy is not currently in attacking stance)
	if (this->getChildByName("charge") != nullptr)
	{
		this->removeChild(m_charge, false);
	}

	// Stops moving
	this->move(Character::DIRECTION::NONE);

	// Stops all running schedulers that may trigger
	unschedule("move_right");
	unschedule("move_left");
	unschedule("waiting_re_finding_player");
	unschedule("move_into_patrol_area");

	// Reset flags (for stance and found-player status
	_state = Character::STATE::IDLE;
	m_attackStance = false;

	// After 2.0 seconds -> set found-player status to false -> continue searching
	scheduleOnce([this](float delta) {

		m_noticePlayer = false;

		this->getQuestionMark()->setVisible(true);		// show indicative sign

	}, DELAY_TIME_FOR_LOOKING_PLAYER, "waiting_re_finding_player");
	
	
	// *Maybe some sounds*

	auto turnAction = ScaleBy::create(0.0f, -1.0f, +1.0f);
	auto delayAction = DelayTime::create(DELAY_TIME_FOR_LOOKING_PLAYER);
	auto sequence = Sequence::createWithTwoActions(delayAction, turnAction);
	auto repeatAction = Repeat::create(sequence->clone(), TIMES_LOOKING_AROUND);

	repeatAction->setTag(REPEAT_ACTION_TAG);

	this->runAction(repeatAction);

	_lookingAround = true;		// set state: looking for Player

	// After that -> move this enemy into position for patrol (again), and patrol [if still not find Player]
	scheduleOnce([&](float delta) {

		if (!m_noticePlayer)
		{
			// Turns on moving flag
			setMovingFlag(true);

			setLookingAroundState(false);	// reset state looking around

			this->moveToPatrolArea();
		}

	}, DELAY_TIME_FOR_LOOKING_PLAYER * TIMES_LOOKING_AROUND + 0.5, "move_into_patrol_area");
}

void EnemyApeDino::onDie()
{
	// Stops EnemyApeDino
	move(DIRECTION::NONE);

	// Stops running schedulers
	unscheduleAllCallbacks();
	stopAllActions();

	// Removes Charge projectile when EnemyApeDino dies
	if (this->getChildByName("charge") != nullptr)
	{
		this->removeChild(m_charge, false);
	}

	// Sets sprite to normal state, and hide all indicative sign (for status)
	_sprite->initWithSpriteFrameName("ape_dino0.png");
	_sprite->setAnchorPoint(Vec2(0.5, 0));
	_questionMark->setVisible(false);
	_exclamationMark->setVisible(false);


	// Calls super
	Enemy::onDie();

	//if (drawNode)
	//{
	//	Director::getInstance()->getRunningScene()->removeChild(drawNode);
	//}
}

void EnemyApeDino::moveToPatrolArea()
{
	auto currentPosition = getPosition();

	// First -> setup all the properties (for indicating direction) to match current direction of this object
	if (getScaleX() == SCALE_X_LEFT)
	{
		_direction = DIRECTION::LEFT;
	}
	else
	{
		_direction = DIRECTION::RIGHT;
	}

	// Current position NOT on the same altitude with patrol endpoints
	if (abs(currentPosition.y - m_start.y) >= 100)
	{
		log("IMPOSSIBLE to return to the patrol area\nAGGRESSIVE actions need to be taken");
		setPosition(m_start);
	}
	else
	{
		auto lower = m_start.x > m_end.x ? m_end : m_start;
		auto higher = m_start.x > m_end.x ? m_start : m_end;

		// If currently below lower endpoint
		if (currentPosition.x < lower.x)
		{
			if (getScaleX() == +1)	// if currently facing LEFT
			{
				// Turns RIGHT
				this->setScaleX(-1);
				_direction = DIRECTION::RIGHT;
			}
		}

		// If currently above higher endpoint
		else if (currentPosition.x > higher.x)
		{
			if (getScaleX() == -1)	// if currently facing RIGHT
			{
				// Turns LEFT
				this->setScaleX(+1);
				_direction = DIRECTION::LEFT;
			}
		}
	}

	// Resets the frame for animation (back to walking-frame)
	if (m_animation->getDelayPerUnit() != DELAY_FRAME_WALK)
	{
		m_animation->setDelayPerUnit(DELAY_FRAME_WALK);

		CC_SAFE_RELEASE(m_action);		// releases previous animation (attacking stance)
		m_action = RepeatForever::create(Animate::create(m_animation));		// animation for walking stance
		CC_SAFE_RETAIN(m_action);
	}

	// Turns off question mark sign
	_questionMark->setVisible(false);

	// Makes object patrol
	patrol();
}

void EnemyApeDino::startAction()
{
	patrol();
}
