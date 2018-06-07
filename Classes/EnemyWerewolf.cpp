#include "EnemyWerewolf.h"

#include "WorldScene.h"

#define HP 30.0f
#define SPEED 50.0f
#define JUMP_SPEED 400.0f
#define HURTING_TIME 0.2f
#define DYING_TIME 3.0f

#define NUMBER_FRAMES_WALK_ANIMATION 4
#define NUMBER_FRAMES_ATTACK_ANIMATION 5

#define DELAY_UNIT_WALK_FRAMES 0.15f
#define DELAY_UNIT_ATTACK_FRAMES 0.12f
#define DELAY_TIME_RE_LOOKING 0.2f
#define DELAY_TIME_TURN_AROUND 0.8f

#define IDLE_SPRITE_FILE_NAME "werewolf_idle.png"

#define SPRITE_SCALE_FACTOR 0.6f
#define WIDTH_SCALE_FACTOR 0.22f
#define HEIGHT_SCALE_FACTOR 0.55f

#define SCALE_X_LEFT +1
#define SCALE_X_RIGHT -1

#define DISTANCE_TO_ATTACK 60.0f
#define DELAY_TIME_ATTACK 0.5f
#define DELAY_TIME_RESET_ATTACK_STATE 2.0f

#define MAX_TURN_AROUND_ACTIONS 5

USING_NS_CC;


EnemyWerewolf::~EnemyWerewolf()
{
	// Releases animations
	CC_SAFE_RELEASE_NULL(_walkAnimation);
	CC_SAFE_RELEASE_NULL(_attackAnimation);
	CC_SAFE_RELEASE_NULL(m_clawAttack);
}

bool EnemyWerewolf::init()
{
	// ------------------------------
	// REMEBER TO SET INITIAL POINT
	// ------------------------------

	auto scaleFactor = Director::getInstance()->getContentScaleFactor();

	// Super init
	if (!Enemy::init())
	{
		return false;
	}


	// Initializes properties
	_hp = HP;
	_maxHP = HP;

	_speed = SPEED * 1.0f / scaleFactor;
	_jumpSpeed = JUMP_SPEED * 1.0f / scaleFactor;

	_charType = Character::CHARACTER_TYPE::ENEMY_WEREWOLF;
	_team = 2;		// enemy team

	_direction = Character::DIRECTION::LEFT;		// temporary
	_pivotPosition = Vec2::ZERO;					// temporary

	_hurtTime = HURTING_TIME;
	//_dieTime = DYING_TIME;

	_state = STATE::IDLE;

	_foundPlayer = false;
	_isAttack = false;
	_hasTurnScheduler = false;
	_isReturning = false;

	_numLegacy = 2;

	_numTurns = 0;					// initial number of done turn around = 0

	// Anchor point
	this->setAnchorPoint(Vec2(0.5, 0));


	return true;
}

void EnemyWerewolf::initSprites()
{
	// ----------------------------------- INITIAL SPRITE ---------------------------------------

	_sprite = Sprite::createWithSpriteFrameName(IDLE_SPRITE_FILE_NAME);
	_sprite->setScale(SPRITE_SCALE_FACTOR);

	this->addChild(_sprite);

	// Content size for EnemyWerewolf Node
	auto size = Size(_sprite->getBoundingBox().size.width * WIDTH_SCALE_FACTOR, _sprite->getBoundingBox().size.height * HEIGHT_SCALE_FACTOR);
	this->setContentSize(size);

	// Sets position for sprite relative to parent node
	_sprite->setAnchorPoint(Vec2(0.5, 0));
	_sprite->setNormalizedPosition(Vec2(0.3, -0.1));


	// ------------------------------------ ANIMATIONS -----------------------------------------

	char str[30];
	Vector<SpriteFrame*> animFrames;
	Animation* animation;

	auto spritecache = SpriteFrameCache::getInstance();

	// Walk Animation
	for (int i = 0; i < NUMBER_FRAMES_WALK_ANIMATION; ++i)
	{
		sprintf(str, "werewolf_walk%d.png", i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}
	animation = Animation::createWithSpriteFrames(animFrames, DELAY_UNIT_WALK_FRAMES);

	_walkAnimation = RepeatForever::create(Animate::create(animation));
	CC_SAFE_RETAIN(_walkAnimation);


	animFrames.clear();


	// Attack Animation
	for (int i = 0; i < NUMBER_FRAMES_ATTACK_ANIMATION; ++i)
	{
		sprintf(str, "werewolf_attack%d.png", i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}
	animFrames.pushBack(spritecache->getSpriteFrameByName("werewolf_attack0.png"));
	animation = Animation::createWithSpriteFrames(animFrames, DELAY_UNIT_ATTACK_FRAMES);

	_attackAnimation = Repeat::create(Animate::create(animation), 1);
	CC_SAFE_RETAIN(_attackAnimation);
}

void EnemyWerewolf::initPhysics()
{
	// Physics Body
	_body = PhysicsBody::createBox(getBoundingBox().size, PhysicsMaterial(1000.0f, 0.0f, 0.0f));

	_body->setDynamic(true);
	_body->setRotationEnable(false);	// avoid rotation when collides at corner

	this->setPhysicsBody(_body);

	// Sets bismask(s)
	setColliderBitMask();
}

void EnemyWerewolf::initProjectiles()
{
	// Claw Projectile
	if (m_clawAttack == nullptr)
	{
		m_clawAttack = dynamic_cast<ClawAttack*>(Projectile::createProjectile(Projectile::PROJECTILE_TYPE::CLAW_ATTACK, this));
		m_clawAttack->setAnchorPoint(Vec2(1, 0.2));
		m_clawAttack->setPosition(Vec2(getContentSize().width * -0.1, getContentSize().height * 0.35f));
		m_clawAttack->setName("claw");

		CC_SAFE_RETAIN(m_clawAttack);
	}
}

void EnemyWerewolf::setPivotPosition(cocos2d::Vec2 pos, Character::DIRECTION initialDirection)
{
	CCASSERT(initialDirection != Character::DIRECTION::NONE, "Initial direction should not be NONE");

	// Sets pivot position
	_pivotPosition = pos;
	this->setPosition(_pivotPosition);

	// Initializes attached Projectile(s)
	initProjectiles();

	// Sets pivot direction
	_pivotDirection = initialDirection;
	_direction = _pivotDirection;
	setScaleX(-1.0f * (int)_direction);

	// Sets starting position for raycast (and its length)
	_startRayHigh = getPosition() + Vec2(getBoundingBox().size.width * 0.51, getBoundingBox().size.height * 0.7);
	_startRayLow = getPosition() + Vec2(getBoundingBox().size.width * 0.51, getBoundingBox().size.height * 0.2);
	_distance = Size(Director::getInstance()->getVisibleSize().width * 0.90f, 0);
}

void EnemyWerewolf::move(Character::DIRECTION direction)
{
	// Stops current action first (if any)
	if (_sprite->getNumberOfRunningActions() > 0)
	{
		_sprite->stopAction(_action);
	}

	if (direction == DIRECTION::NONE)
	{
		_sprite->initWithSpriteFrameName(IDLE_SPRITE_FILE_NAME);
		_sprite->setAnchorPoint(Vec2(0.5, 0));
	}
	else if (direction == DIRECTION::LEFT || direction == DIRECTION::RIGHT)
	{
		CC_ASSERT(direction == _direction);

		// Runs animation for walking
		_action = _walkAnimation;
		_sprite->runAction(_action);
	}

	// Calls super
	Enemy::move(direction);
}

void EnemyWerewolf::attackPhysical()
{
	// Stops current action first (if any)
	if (_sprite->getNumberOfRunningActions() > 0)
	{
		_sprite->stopAction(_action);
	}

	// Executes attack animation
	_action = _attackAnimation;
	_sprite->runAction(_action);

	AUDIO::play2d("grr.ogg");

	// Adds projectile
	scheduleOnce([&](float delta) {

		if (this->getChildByName("claw") == nullptr)
		{
			this->addChild(m_clawAttack);
			m_clawAttack->showEffects();
		}
		else
		{
			CCLOG("attackPhysical(): Claw projectile had been already added");
		}

	}, _action->getDuration() * 0.4f, "start_attack_scheduler");

	// Schedules to remove projectile when attack ends
	scheduleOnce([&](float delta) {

		if (this->getChildByName("claw") != nullptr)
		{
			this->removeChildByName("claw", false);
			m_clawAttack->hideEffects();
		}

		//if ((this->getParent())->getChildByName("claw") != nullptr)
		//{
		//	(this->getParent())->removeChildByName("claw", false);
		//}

	}, _action->getDuration() * 0.7f, "end_attack_scheduler");
}

void EnemyWerewolf::onDie()
{
	// Animation for dying??

	// Stops all running schedulers
	unscheduleAllCallbacks();

	move(DIRECTION::NONE);

	// Removes Claw projectile when EnemyWerewolf dies
	if (this->getChildByName("claw") != nullptr)
	{
		this->removeChildByName("claw", false);
		m_clawAttack->hideEffects();
	}

	// Sets to IDLE sprite
	_sprite->initWithSpriteFrameName(IDLE_SPRITE_FILE_NAME);
	_sprite->setAnchorPoint(Vec2(0.5, 0));

	// Calls super
	Enemy::onDie();
}

//DrawNode* drawNode2;

void EnemyWerewolf::update(float delta)
{
	// DEBUG
	//if (drawNode2)
	//{
	//	Director::getInstance()->getRunningScene()->removeChild(drawNode2);
	//}
	//drawNode2 = DrawNode::create();
	//Director::getInstance()->getRunningScene()->addChild(drawNode2);


	// ================================================== RETURN TO PIVOT POSITION ==========================================

	// IF reaches the max number of turn around actions (without seeing Player of course) -> RETURN to the PIVOT postion
	if (getNumTurns() == MAX_TURN_AROUND_ACTIONS)
	{
		setReturningState(true);
		unschedule("turn_around_scheduler");
		setTurnAroundScheduler(false);
		resetTurnActions();

		// Turns EnemyWerewolf back
		if (_pivotPosition.x < getPosition().x && _direction == DIRECTION::RIGHT)		// pivot position on the LEFT, and currently facing RIGHT
		{
			turnAround();
		}
		else if (_pivotPosition.x > getPosition().x && _direction == DIRECTION::LEFT)	// pivot position on the RIGHT, and currently facing LEFT
		{
			turnAround();
		}

		move(_direction);
	}

	// IF is returning to pivot position
	if (_isReturning)
	{
		// IF reaches pivot position
		if ((_direction == DIRECTION::LEFT && getPosition().x <= _pivotPosition.x) || _direction == DIRECTION::RIGHT && getPosition().x >= _pivotPosition.x)
		{

			// Stops
			move(DIRECTION::NONE);

			setPosition(_pivotPosition);
			if (_direction != _pivotDirection)
			{
				turnAround();
			}

			resetStartAction();
			setReturningState(false);

			return;
		}
	}

	// =================================================== END OF SECTION ================================================


	// ======================================== POSITION TO ATTACK =============================================

	/**
	* Monitors the relative position of EnemyWerewolf to Player => to ATTACK
	*/

	// IF is preparing to attack -> DO NOT move AND search for Player
	if (_isAttack)
	{
		if (_state != STATE::IDLE)
		{
			move(DIRECTION::NONE);
		}

		return;
	}
	else if (!_isReturning && _state == STATE::RUN)
	{
		auto scaleFactor = Director::getInstance()->getContentScaleFactor();
		//auto world = World::getInstance();
		auto world = World::getCurrent();
		auto player = world->getPlayer();

		// Determines the vantage points for EnemyWerewolf and Player to compare
		float playerX, enemyX;
		if (getPosition().x < player->getPosition().x)
		{
			// In World layer
			auto worldPlayerPos = player->getPosition() - Vec2(player->getBoundingBox().size.width / 2, 0);
			auto worldEnemyPos = getPosition() + Vec2(getBoundingBox().size.width / 2, 0);

			// In running Scene
			auto scenePlayerPos = world->convertToWorldSpace(worldPlayerPos);
			auto sceneEnemyPos = world->convertToWorldSpace(worldEnemyPos);

			//drawNode2->drawDot(scenePlayerPos, 3.0f, Color4F::BLUE);
			//drawNode2->drawDot(sceneEnemyPos, 3.0f, Color4F::BLUE);


			playerX = scenePlayerPos.x;
			enemyX = sceneEnemyPos.x;
		}
		else
		{
			// In World layer
			auto worldPlayerPos = player->getPosition() + Vec2(player->getBoundingBox().size.width / 2, 0);
			auto worldEnemyPos = getPosition() - Vec2(getBoundingBox().size.width / 2, 0);

			// In running Scene
			auto scenePlayerPos = world->convertToWorldSpace(worldPlayerPos);
			auto sceneEnemyPos = world->convertToWorldSpace(worldEnemyPos);

			//drawNode2->drawDot(scenePlayerPos, 3.0f, Color4F::BLUE);
			//drawNode2->drawDot(sceneEnemyPos, 3.0f, Color4F::BLUE);


			playerX = scenePlayerPos.x;
			enemyX = sceneEnemyPos.x;
		}

		// Within "safe" range to attack
		if (abs(playerX - enemyX) <= DISTANCE_TO_ATTACK / scaleFactor)
		{
			// Stops moving (towards Player)
			move(DIRECTION::NONE);

			// Sets attacking state
			_isAttack = true;

			// Schedules an attack (after a while)
			scheduleOnce([&](float delta) {

				this->attackPhysical();

			}, DELAY_TIME_ATTACK, "attack_scheduler");

			// Schedules to reset attacking state
			scheduleOnce([&](float delta) {

				this->setAttackingState(false);

				// Returns to IDLE state
				move(DIRECTION::NONE);

			}, DELAY_TIME_RESET_ATTACK_STATE, "reset_attack_state_scheduler");

			return;
		}
	}

	// =============================================== END OF SECTION =======================================================


	// =========================================== RAYCAST ==========================================

	/**
	 * Monitors the eyesight of EnemyWerewolf by using ray cast (in physics engine)
	 *
	 * NOTE: ONLY when EnemyWerewolf is not in attacking state
	 */

	// Updates endpoints for raycasting
	Vec2 endRayHigh, endRayLow;
	if (getScaleX() == SCALE_X_LEFT)
	{
		_startRayHigh.x = getPosition().x - getBoundingBox().size.width * 0.51;
		_startRayLow.x = getPosition().x - getBoundingBox().size.width * 0.51;

		endRayHigh = _startRayHigh - _distance;
		endRayLow = _startRayLow - _distance;
	}
	else if (getScaleX() == SCALE_X_RIGHT)
	{
		_startRayHigh.x = getPosition().x + getBoundingBox().size.width * 0.51;
		_startRayLow.x = getPosition().x + getBoundingBox().size.width * 0.51;

		endRayHigh = _startRayHigh + _distance;
		endRayLow = _startRayLow + _distance;
	}


	// ----------------------------------- FIRST -----------------------------------------

	_bodies.clear();
	Director::getInstance()->getRunningScene()->getPhysicsWorld()->rayCast(
		CC_CALLBACK_3(EnemyWerewolf::physicsRayCastFunc, this), _startRayHigh, endRayHigh, nullptr);

	//DEBUG
	//drawNode2->drawSegment(_startRayHigh, endRayHigh, 1, Color4F::RED);
	//drawNode2->drawSegment(_startRayLow, endRayLow, 1, Color4F::RED);


	// Checks list of detected bodies
	Vec2 closestBlock = Vec2(-999.0f, -999.0f);
	Vec2 playerPos = Vec2(-999.0f, -999.0f);
	//Vec2 enemyPos = World::getInstance()->convertToWorldSpace(getPosition());
	Vec2 enemyPos = World::getCurrent()->convertToWorldSpace(getPosition());

	for (auto iter = _bodies.cbegin(); iter != _bodies.cend(); ++iter)
	{
		// THIS ALGORITHM BASED ON THE ORDER OF DECTECTION OF RAYCAST -> NOT WORK !!!!!!!!!

		//// IF a Block object is between EnemyWerewolf and Player -> CANNOT SEE
		//if ((*iter)->getCategoryBitmask() == GameObject::OBJECT_TYPE::BLOCK)
		//{
		//	setFoundPlayer(false);
		//	break;						// stops searching for Player
		//}

		//else if ((*iter)->getCategoryBitmask() == GameObject::OBJECT_TYPE::CHARACTER)
		//{
		//	auto character = dynamic_cast<Character*>((*iter)->getOwner());

		//	// IF Player
		//	if (character->getTeam() != _team)
		//	{
		//		// Start action (if not already)
		//		if (!_startAction)
		//		{
		//			_startAction = true;
		//		}

		//		// Player found
		//		if (!_foundPlayer)
		//		{
		//			setFoundPlayer(true);
		//		}

		//		// Turns off scheduler to turn EnemyWerewolf around (looking for Player)
		//		unschedule("turn_around_scheduler");
		//		setTurnAroundScheduler(false);
		//		resetTurnActions();

		//		// If is standing -> moves towards Player
		//		if (_state == STATE::IDLE)
		//		{
		//			move(_direction);
		//		}

		//		break;					// stops searching for Player
		//	}
		//}


		//drawNode2->drawDot((*iter)->getPosition(), 3.0f, Color4F::GREEN);


		// RETRIEVES ALL BLOCKS -> FIND THE CLOSEST TO ENEMYWEREWOLF -> COMPARE TO PLAYER

		// IF detects BLOCK object
		if ((*iter)->getCategoryBitmask() == GameObject::OBJECT_TYPE::BLOCK)
		{
			// First one -> set to the closest BLOCK
			if (closestBlock == Vec2(-999, -999))
			{
				closestBlock = (*iter)->getPosition();
			}
			else
			{
				// Find the closest one to EnemyWerewolf
				if (abs((*iter)->getPosition().x - enemyPos.x) < abs(closestBlock.x - enemyPos.x))
				{
					closestBlock = (*iter)->getPosition();
				}
			}
		}

		// IF detects Player
		else if ((*iter)->getCategoryBitmask() == GameObject::OBJECT_TYPE::CHARACTER)
		{
			auto character = dynamic_cast<Character*>((*iter)->getOwner());

			if (character->getTeam() != _team)
			{
				playerPos = (*iter)->getPosition();
			}
		}
	}

	// Consider each body: IF there is Block between EnemyWerewolf and Player -> CANNOT SEE PLAYER
	if (playerPos == Vec2(-999.0f, -999.0f))
	{
		setFoundPlayer(false);								// SEE NO PLAYER
	}
	else if (closestBlock != Vec2(-999.0f, -999.0f) && abs(playerPos.x - enemyPos.x) > abs(closestBlock.x - enemyPos.x))
	{
		setFoundPlayer(false);								// SEE PLAYER, WITH BLOCK BETWEEN 
	}
	else if (closestBlock == Vec2(-999.0f, -999.0f) || abs(playerPos.x - enemyPos.x) < abs(closestBlock.x - enemyPos.x))
	{
															// SEE PLAYER, NO BLOCK || SEE PLAYER, WITH BLOCK BUT NOT BETWEEN
		// Start action (if not already)
		if (!_startAction)
		{
			_startAction = true;
		}

		// Player found
		if (!_foundPlayer)
		{
			setFoundPlayer(true);
		}

		// IF is returning -> STOPS
		if (_isReturning)
		{
			setReturningState(false);
		}

		// Turns off scheduler to turn EnemyWerewolf around (looking for Player)
		unschedule("turn_around_scheduler");
		setTurnAroundScheduler(false);
		resetTurnActions();

		// If is standing -> moves towards Player
		if (_state == STATE::IDLE)
		{
			move(_direction);
		}
	}

	// -------------------------------------- END -------------------------------------------


	// -------------------------------------- SECOND -----------------------------------------

	if (!_foundPlayer)
	{
		_bodies.clear();
		Director::getInstance()->getRunningScene()->getPhysicsWorld()->rayCast(
			CC_CALLBACK_3(EnemyWerewolf::physicsRayCastFunc, this), _startRayLow, endRayLow, nullptr);

		// Checks list of detected bodies
		closestBlock = Vec2(-999.0f, -999.0f);
		playerPos = Vec2(-999.0f, -999.0f);

		for (auto iter = _bodies.cbegin(); iter != _bodies.cend(); ++iter)
		{
			// THIS ALGORITHM BASED ON THE ORDER OF DECTECTION OF RAYCAST -> NOT WORK !!!!!!!!!

			//// IF a Block object is between EnemyWerewolf and Player -> CANNOT SEE
			//if ((*iter)->getCategoryBitmask() == GameObject::OBJECT_TYPE::BLOCK)
			//{
			//	setFoundPlayer(false);
			//	break;						// stops searching for Player
			//}

			//else if ((*iter)->getCategoryBitmask() == GameObject::OBJECT_TYPE::CHARACTER)
			//{
			//	auto character = dynamic_cast<Character*>((*iter)->getOwner());

			//	// IF Player
			//	if (character->getTeam() != _team)
			//	{
			//		// Start action (if not already)
			//		if (!_startAction)
			//		{
			//			_startAction = true;
			//		}

			//		// Player found
			//		if (!_foundPlayer)
			//		{
			//			setFoundPlayer(true);
			//		}

			//		// Turns off scheduler to turn EnemyWerewolf around (looking for Player)
			//		unschedule("turn_around_scheduler");
			//		setTurnAroundScheduler(false);
			//		resetTurnActions();

			//		// If is standing -> moves towards Player
			//		if (_state == STATE::IDLE)
			//		{
			//			move(_direction);
			//		}

			//		break;					// stops searching for Player
			//	}
			//}

			//drawNode2->drawDot((*iter)->getPosition(), 3.0f, Color4F::GREEN);


			// RETRIEVES ALL BLOCKS -> FIND THE CLOSEST TO ENEMYWEREWOLF -> COMPARE TO PLAYER

			// IF detects BLOCK object
			if ((*iter)->getCategoryBitmask() == GameObject::OBJECT_TYPE::BLOCK)
			{
				// First one -> set to the closest BLOCK
				if (closestBlock == Vec2(-999.0f, -999.0f))
				{
					closestBlock = (*iter)->getPosition();
				}
				else
				{
					// Find the closest one to EnemyWerewolf
					if (abs((*iter)->getPosition().x - enemyPos.x) < abs(closestBlock.x - enemyPos.x))
					{
						closestBlock = (*iter)->getPosition();
					}
				}
			}

			// IF detects Player
			else if ((*iter)->getCategoryBitmask() == GameObject::OBJECT_TYPE::CHARACTER)
			{
				auto character = dynamic_cast<Character*>((*iter)->getOwner());

				if (character->getTeam() != _team)
				{
					playerPos = (*iter)->getPosition();
				}
			}
		}

		// Consider each body: IF there is Block between EnemyWerewolf and Player -> CANNOT SEE PLAYER
		if (playerPos == Vec2(-999.0f, -999.0f))
		{
			setFoundPlayer(false);								// SEE NO PLAYER
		}
		else if (closestBlock != Vec2(-999.0f, -999.0f) && abs(playerPos.x - enemyPos.x) > abs(closestBlock.x - enemyPos.x))
		{
			setFoundPlayer(false);								// SEE PLAYER, WITH BLOCK BETWEEN 
		}
		else if (closestBlock == Vec2(-999.0f, -999.0f) || abs(playerPos.x - enemyPos.x) < abs(closestBlock.x - enemyPos.x))
		{
																// SEE PLAYER, NO BLOCK || SEE PLAYER, WITH BLOCK BUT NOT BETWEEN
			// Start action (if not already)
			if (!_startAction)
			{
				_startAction = true;
			}

			// Player found
			if (!_foundPlayer)
			{
				setFoundPlayer(true);
			}

			// IF is returning -> STOPS
			if (_isReturning)
			{
				setReturningState(false);
			}

			// Turns off scheduler to turn EnemyWerewolf around (looking for Player)
			unschedule("turn_around_scheduler");
			setTurnAroundScheduler(false);
			resetTurnActions();

			// If is standing -> moves towards Player
			if (_state == STATE::IDLE)
			{
				move(_direction);
			}
		}
	}

	// ------------------------------------------ END --------------------------------------------


	// ONLY when EnemyWerewolf has started its actions
	if (_startAction && !_isReturning)
	{
		// If not see Player -> stops moving
		if (!_foundPlayer)
		{
			move(DIRECTION::NONE);
		}

		// IF not see Player for a while -> turn around to look
		if (!_foundPlayer && !_hasTurnScheduler)
		{
			setTurnAroundScheduler(true);

			scheduleOnce([&](float delta) {

				turnAround();
				setTurnAroundScheduler(false);

				// Increase number of turn around actions
				this->increaseTurnAction();

			}, DELAY_TIME_TURN_AROUND, "turn_around_scheduler");
		}
	}

	// ======================================== END OF SECTION ===========================================
}

bool EnemyWerewolf::physicsRayCastFunc(cocos2d::PhysicsWorld& world, const cocos2d::PhysicsRayCastInfo& info, void* data)
{
	//auto ptrVec2 = (Vec2*)data;
	//(*ptrVec2) = info.contact;

	// Stores all PhysicsBodies detected
	_bodies.pushBack(info.shape->getBody());


	return true;
}

void EnemyWerewolf::increaseTurnAction()
{
	++_numTurns;
	if (_numTurns > MAX_TURN_AROUND_ACTIONS)
	{
		_numTurns = MAX_TURN_AROUND_ACTIONS;
	}
}

void EnemyWerewolf::resetTurnActions()
{
	_numTurns = 0;
}

void EnemyWerewolf::resetStartAction()
{
	_startAction = false;
}

// EnemyWerewolf will starts its action if being hit
void EnemyWerewolf::onHit(float dmg, DIRECTION dir, float force)
{
	// Super
	Enemy::onHit(dmg, dir, force);

	// IF is stationary -> starts looking for Player
	if (!_startAction)
	{
		_startAction = true;
	}

	// IF is returning to pivot position -> stops + starts looking for Player
	else if (_isReturning)
	{
		setReturningState(false);
	}
}
