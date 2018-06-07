#include "Player.h"

#include "Ground.h"
#include "PushableBlock.h"

#include "ResourceManager.h"
#include "HUDLayer.h"

#include "MyPoolManager.h"

#include "CameraHelper.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		#include "ui/CocosGUI.h"
#else
		#include "ui\CocosGUI.h"
#endif


USING_NS_CC;

#define PLAYER_OVERLAP_Y_LIMIT 3.0f

#define PLAYER_HURT_TIME 0.3f
#define PLAYER_HURT_VERTICAL_DISTANCE 350.0f
#define PLAYER_HURT_DIAGONAL_DISTANCE 150.0f

#define PLAYER_DIED_TIME 1.0f


#define PLAYER_DIAGONAL_STANDABLE 0.8f

#define PLAYER_NUMBER_SKILLS 5

#define PLAYER_MAX_COOLDOWN_0 0.5f
#define PLAYER_MAX_COOLDOWN_1 1.0f
#define PLAYER_MAX_COOLDOWN_2 5.0f
#define PLAYER_MAX_COOLDOWN_3 5.0f
//#define PLAYER_MAX_COOLDOWN_4 1.0f no need

// release all animations
Player::~Player()
{
	act_idle->release();
	act_walk->release();
	act_jump->release();

	act_attack->release();
	act_attack2->release();

	act_hurt->release();
	act_die->release();

	delete[] maxCooldowns;
	delete[] cooldowns;
	delete[] isReadys;
}

void Player::onExit()
{
	// Super
	Node::onExit();

	// Registers Player's legacy into UserDefault
	UserDefault::getInstance()->setIntegerForKey("player_legacy", _legacy);

	UserDefault::getInstance()->flush();
}

bool Player::init()
{
	info_contentScaleFactor = Director::getInstance()->getContentScaleFactor();

	if (!Character::init())
		return false;

	_chargeFX = nullptr;


	_state = STATE::IDLE;
	_moveDirect = 0;

	_hp = _maxHP;

	_isAttack = false;

	////////////////////////////////////
	// Cooldowns
	maxCooldowns = new float[PLAYER_NUMBER_SKILLS];
	maxCooldowns[0] = PLAYER_MAX_COOLDOWN_0;
	maxCooldowns[1] = PLAYER_MAX_COOLDOWN_1;
	maxCooldowns[2] = PLAYER_MAX_COOLDOWN_2;
	maxCooldowns[3] = PLAYER_MAX_COOLDOWN_3;

	cooldowns = new float [PLAYER_NUMBER_SKILLS];
	isReadys = new bool[PLAYER_NUMBER_SKILLS];
	for (int i = 0; i < (PLAYER_NUMBER_SKILLS - 1); i++)
	{
		cooldowns[i] = maxCooldowns[i];
		isReadys[i] = true;
	}

	// ------------------------------------

	//_camera = nullptr;
	_movingBlockBody = nullptr;

	_hasGround = false;


	// Extract "total" legacy
	_legacy = UserDefault::getInstance()->getIntegerForKey("player_legacy", 0);

	return true;
}


// inherited class call this in initPhysics()
void Player::setColliderBitMask()
{
	if (_body == nullptr)
		return;

	_body->setCategoryBitmask((int)OBJECT_TYPE::CHARACTER);// character
	_body->setCollisionBitmask(
		(int)OBJECT_TYPE::CHARACTER
		| (int)OBJECT_TYPE::BLOCK
		| (int)OBJECT_TYPE::ZONE
	);
	_body->setContactTestBitmask(
		(int)OBJECT_TYPE::CHARACTER
		| (int)OBJECT_TYPE::PROJECTILE
		| (int)OBJECT_TYPE::BLOCK
		| (int)OBJECT_TYPE::ITEM
		| (int)OBJECT_TYPE::ZONE
	);
}

// callback when keeping collide with something
bool Player::onContactPresolve(const cocos2d::PhysicsContact &contact)
{
	// Retrieves PhysicsBody of target
	auto targetBody = contact.getShapeA()->getBody();

	if (_body == targetBody)		// if current body is of shape A
	{
		targetBody = contact.getShapeB()->getBody();
	}

	
	Vec2 normal = contact.getContactData()->normal;
	// target is block
	if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::BLOCK)
	{
		Block* block = (Block*)targetBody->getOwner();

		tmp_tarPos = targetBody->getPosition();
		tmp_myPos = _body->getPosition();
		
		Block::BLOCK_TYPE typeBlk = block->getBlockType();

			// one way platform
		if (block->getOneWay())
		{
			bool check1 = (tmp_myPos.y - getBoundingBox().size.height / 2.0f + PLAYER_OVERLAP_Y_LIMIT / Director::getInstance()->getContentScaleFactor()) < (tmp_tarPos.y + block->size.height / 2.0f);
			bool check2 = (normal.y < -PLAYER_DIAGONAL_STANDABLE || normal.y > PLAYER_DIAGONAL_STANDABLE);
			if ( check1 && check2)
			{
				return false;
			}
		}

		// if the block collide in horizontal => push into array[] for handle after
		if (normal.y == 0)
		{
			if ((_moveDirect < 0 && tmp_tarPos.x < tmp_myPos.x)
				|| (_moveDirect > 0 && tmp_tarPos.x > tmp_myPos.x))
			{
				if (typeBlk == Block::BLOCK_TYPE::PUSHABLE && _state == STATE::RUN)
				{
					//targetBody->setVelocity(Vec2(_speed * _moveDirect, 0) * _forceFactor);
					((PushableBlock*)block)->push(_speed * _moveDirect * _forceFactor);
				}

				m_blockBodies.pushBack(targetBody);
			}
		}
		else
		{
			if ((normal.y < -PLAYER_DIAGONAL_STANDABLE || normal.y > PLAYER_DIAGONAL_STANDABLE) && (tmp_myPos.y > tmp_tarPos.y))
			{
				_hasGround = true;
				if (_state == STATE::JUMP || _state == STATE::DOUBLE_JUMP)
				{
					if (_isAttack)
					{
						_state = STATE::IDLE;
						_vecMove = Vec2::ZERO;
						_body->setVelocity(_vecMove);
					}
					else if (_moveDirect == 0)
					{
						endWalk();
					}
					else
					{
						_vecMove.y = 0;
						startWalk();
					}
				}
			}

			if (typeBlk == Block::BLOCK_TYPE::MOVING )//|| typeBlk == Block::BLOCK_TYPE::MOVING_BY_TRIGGER)
			{
				_movingBlockBody = targetBody;//block->getBody();
			}
		}
	}
	else if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::CHARACTER)
	{
		tmp_tarPos = targetBody->getPosition();
		tmp_myPos = _body->getPosition();

		if ((normal.y < -PLAYER_DIAGONAL_STANDABLE || normal.y > PLAYER_DIAGONAL_STANDABLE) && (tmp_myPos.y > tmp_tarPos.y))
		{
			/*_hasGround = true;
			if (_state == STATE::JUMP || _state == STATE::DOUBLE_JUMP)
			{
				if (_isAttack)
				{
					_state = STATE::IDLE;
					_vecMove = Vec2::ZERO;
					_body->setVelocity(_vecMove);
				}
				else if (_moveDirect == 0)
				{
					endWalk();
				}
				else
				{
					_vecMove.y = 0;
					startWalk();
				}
			}
			_movingBlockBody = targetBody;*/
			return false;
		}
	}

	return true;
}

// update camera position in main update() func
//void Player::updateCamera()
//{
//	//if (_camera == nullptr)
//	//{
//	//	_camera = Camera::getDefaultCamera();
//
//	//	info_mapRect = ResourceManager::getInstance()->getArea();
//	//	info_visibleSize = Director::getInstance()->getVisibleSize();
//	//}
//
//	//// camera position based on this player
//	//tmp_vec = this->getPosition();
//
//	//// --- fix position by map-bounding ---
//
//	//// if width of map > width of screen
//	//if (info_mapRect.size.width > info_visibleSize.width)
//	//{
//	//	// outside the bouding? move into bounding
//	//	if (tmp_vec.x < info_mapRect.getMinX() + info_visibleSize.width / 2)
//	//	{
//	//		tmp_vec.x = info_mapRect.getMinX() + info_visibleSize.width / 2;
//	//	}
//	//	else if (tmp_vec.x > info_mapRect.getMaxX() - info_visibleSize.width / 2)
//	//	{
//	//		tmp_vec.x = info_mapRect.getMaxX() - info_visibleSize.width / 2;
//	//	}
//	//}
//	//else // width of map < width of screen => fix 1 position x
//	//{
//	//	tmp_vec.x = info_mapRect.getMinX() + info_visibleSize.width / 2;
//	//}
//
//	//// if height of map > height of screen
//	//if (info_mapRect.size.height > info_visibleSize.height)
//	//{
//	//	// outside the bouding? move into bounding
//	//	if (tmp_vec.y < info_mapRect.getMinY() + info_visibleSize.height / 2)
//	//	{
//	//		tmp_vec.y = info_mapRect.getMinY() + info_visibleSize.height / 2;
//	//	}
//	//	else if (tmp_vec.y > info_mapRect.getMaxY() - info_visibleSize.height / 2)
//	//	{
//	//		tmp_vec.y = info_mapRect.getMaxY() - info_visibleSize.height / 2;
//	//	}
//	//}
//	//else // height of map < height of screen => fix 1 position y
//	//{
//	//	tmp_vec.y = info_mapRect.getMinY() + info_visibleSize.height / 2;
//	//}
//
//	//// set camera position after fixed
//	//_camera->setPosition(tmp_vec);
//
//	CameraHelper::getInstance()->updatePosition(_position);
//}


//#define OVERLAP_LIMIT -5.0f
/* Handling overlap block
* if there are blocks collide in left | right => put into m_blockBodies
* in m_blockBodies, there may be many m_blockBodies that overlap this player
* function: prevent player continue to move*/
void Player::handleOverlapBlock()
{
	if (m_blockBodies.size() > 0)
	{
		PhysicsBody* tmpBody;
		float minVelocityX = 999;

		while (m_blockBodies.size() > 0)
		{
			tmpBody = m_blockBodies.back();

			m_blockBodies.popBack();


			if (abs(tmpBody->getVelocity().x) < abs(minVelocityX))
			{
				minVelocityX = tmpBody->getVelocity().x;
			}		
		}

		// many block that collide with player (in horizontal)
		// => prevent player continue to move that the slowest block
		_vecMove.x = minVelocityX;
	}
}


void Player::updateCooldowns(float delta)
{
	for (int i = 0; i < (PLAYER_NUMBER_SKILLS - 1); i++)
	{
		if (isReadys[i])
			continue;

		cooldowns[i] -= delta;

		if (cooldowns[i] <= 0)
		{
			cooldowns[i] = 0;
			isReadys[i] = true;
		}
	}
}


/* moving method of this character
* press_down "right", call move(right)
* press_down "left", call move(left)
** press_up "right", call move(left) * LOL*/
void Player::move(Character::DIRECTION direct)
{
	int dir = _moveDirect;
	dir += (int)direct;

	if (dir == 0)
	{
		_moveDirect = 0;
		if (_state == STATE::RUN)
		{
			endWalk();
		}

		if (_state == STATE::IDLE)
		{
			return;
		}
	}

	// set sprite_scale_X & direction
	if (dir > 0)
	{
		_moveDirect = 1;
	}
	else if (dir < 0)
	{
		_moveDirect = -1;
	}


	// if stand attacking, don't move!
	if (_isAttack && _state == STATE::IDLE)
		return;

	// if hurted, don't move
	if (_state == STATE::HURTED)
		return;


	if (dir != 0)
	{
		this->setScaleX(_moveDirect);

		// reset the direction that character face to
		_direction = (DIRECTION)_moveDirect;
	}

	// idling or jumping, can move
	_vecMove.x = _speed*dir;


	// dir != 0
	if (_state == STATE::IDLE)
	{
		_vecMove.y = _body->getVelocity().y;
		startWalk();
	}

	/*if (_state == STATE::JUMP || _state == STATE::DOUBLE_JUMP)
	{
		_vecMove.y = _body->getVelocity().y;
	}*/
}

/* when idling, start walk
* called by move()*/
void Player::startWalk()
{
	_vecMove.x = _speed* _moveDirect;
	this->setScaleX(_moveDirect);
	_direction = (DIRECTION)_moveDirect;

	_state = STATE::RUN;



	// stop the idle animation
	_sprite->stopAction(_action);

	// run the walk animation
	_action = act_walk;
	_sprite->runAction(_action);
}

/* when walking, stop walk and become idle
* called by move()*/
void Player::endWalk()
{
	_state = STATE::IDLE;

	_vecMove.x = 0;



	_sprite->stopAction(_action);

	_action = act_idle;
	_sprite->runAction(_action);
}

// update() call this
void Player::endAttack()
{
	_isAttack = false;



	if (_state == STATE::IDLE)
	{
		if (_moveDirect == 0)
		{
			_sprite->stopAction(_action);
			_action = act_idle;
			_sprite->runAction(_action);
		}
		else
		{
			startWalk();
		}
	}
}

/* public method for make this character jump
* double jump only*/
void Player::jump()
{
	// attacking? not jump
	if (_isAttack && (_state == STATE::IDLE || _state == STATE::RUN))
		return;

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

	if (_state == STATE::RUN)
	{
		//m_vecMove.x = _body->getVelocity().x;
		_state = STATE::JUMP;
	}

	_vecMove.y = _jumpSpeed;
	_body->setVelocity(_vecMove);

	_isAttack = false;

	_sprite->stopAction(_action);
	_action = act_jump;
	_sprite->runAction(_action);

	AUDIO::play2d("jump.ogg");
}

/* public method for make dmg to this character
* dmg: the amount of damage this character taken
* dir: the direction for this character, face the damager source*/
void Player::onHit(float dmg, DIRECTION dir, float force)
{
	AUDIO::play2d("hurt_2.ogg");
	_hp -= dmg;
	if (_hp < 0) _hp = 0;

	// make knock back
	knock(dir, force);

	// take dmg
	//Character::onHit(dmg, dir);

	
	// Update health bar in HUD layer
	char text[20];
	sprintf(text, "%03.0f/%03.0f", getHP(), getMaxHP());

	// Retrieves HUD layer
	auto hudLayer = dynamic_cast<HUDLayer*>(Director::getInstance()->getRunningScene()->getChildByName("play_scene")->getChildByName("hud_layer"));

	auto hpBar = dynamic_cast<ui::LoadingBar*>(hudLayer->getChildByName("hp_bar"));		// retrieves HP bar
	auto hpLabel = dynamic_cast<Label*>(hudLayer->getChildByName("hp_label"));			// retrieves HP label

	hpLabel->setString(text);								// update HP label
	hpBar->setPercent(getHP() * 100.0f / getMaxHP());		// update HP bar


	// Change Player's avartar on HUD layer based on the current health
	if (_hp <= PLAYER_HEALTH_THRESHOLD_HURT * _maxHP &&
		((_hp + dmg) > PLAYER_HEALTH_THRESHOLD_HURT * _maxHP))		// first threshold
	{
		// Retrieve HUDLayer -> Player's avatar
		auto hudLayer = dynamic_cast<HUDLayer*>(Director::getInstance()->getRunningScene()->getChildByName("play_scene")->getChildByName("hud_layer"));

		//auto sprite = dynamic_cast<Sprite*>(hudLayer->getChildByName("player_avatar"));
		auto avatar = dynamic_cast<ui::Button*>(hudLayer->getChildByName("player_avatar"));

		// Based on Player's type -> set corresponding sprite
		char str[50];
		if (_charType == Character::CHARACTER_TYPE::RED_DINOSAUR)
		{
			sprintf(str, "tyro_status_hurt.png");
		}
		else if (_charType == Character::CHARACTER_TYPE::WHITE_DINOSAUR)
		{
			sprintf(str, "trino_status_hurt.png");
		}
		else if (_charType == Character::CHARACTER_TYPE::YELLOW_DINOSAUR)
		{
			sprintf(str, "ptero_status_hurt.png");
		}
		//sprite->initWithSpriteFrameName(str);
		//sprite->setAnchorPoint(Vec2(0, 1));

		avatar->loadTextures(str, str, str, ui::Widget::TextureResType::PLIST);
		avatar->setAnchorPoint(Vec2(0, 1));
	}
	if (_hp <= PLAYER_HEALTH_THRESHOLD_SERIOUS_HURT * _maxHP &&
		((_hp + dmg) > PLAYER_HEALTH_THRESHOLD_SERIOUS_HURT * _maxHP))	// second threshold -> more serious
	{
		// Retrieve HUDLayer -> Player's avatar
		auto hudLayer = dynamic_cast<HUDLayer*>(Director::getInstance()->getRunningScene()->getChildByName("play_scene")->getChildByName("hud_layer"));

		//auto sprite = dynamic_cast<Sprite*>(hudLayer->getChildByName("player_avatar"));
		auto avatar = dynamic_cast<ui::Button*>(hudLayer->getChildByName("player_avatar"));

		// Based on Player's type -> set corresponding sprite
		char str[50];
		if (_charType == Character::CHARACTER_TYPE::RED_DINOSAUR)
		{
			sprintf(str, "tyro_status_die.png");
		}
		else if (_charType == Character::CHARACTER_TYPE::WHITE_DINOSAUR)
		{
			sprintf(str, "trino_status_die.png");
		}
		else if (_charType == Character::CHARACTER_TYPE::YELLOW_DINOSAUR)
		{
			sprintf(str, "ptero_status_die.png");
		}
		//sprite->initWithSpriteFrameName(str);
		//sprite->setAnchorPoint(Vec2(0, 1));

		avatar->loadTextures(str, str, str, ui::Widget::TextureResType::PLIST);
		avatar->setAnchorPoint(Vec2(0, 1));
	}
}

/* knock back
* direction of player, knocking to
# onHit() call this*/
void Player::knock(DIRECTION dir, float force)
{
	//_isAttack = false;
	endAttack();

	_state = STATE::HURTED;




	_sprite->stopAction(_action);

	if (_hp > 0)
	{
		_timerHurt = PLAYER_HURT_TIME;
		_action = act_hurt;
	}
	else
	{
		_body->getShapes().at(0)->setFriction(10.0f);
		_body->setContactTestBitmask(0);

		_timerHurt = PLAYER_DIED_TIME;
		_action = act_die;
	}



	_sprite->runAction(_action);

	int directHurt = (int)dir;
	// from left or right
	if (dir != DIRECTION::BOTTOM && dir != DIRECTION::TOP)
	{
		_vecMove.x = force * (-directHurt) * PLAYER_HURT_DIAGONAL_DISTANCE / info_contentScaleFactor;
		_vecMove.y = force * PLAYER_HURT_DIAGONAL_DISTANCE / info_contentScaleFactor;

		this->setScaleX(directHurt);
		_direction = (DIRECTION)dir;
	}
	else
	{
		directHurt -= 3;
		_vecMove.x = 0;
		_vecMove.y = force * (directHurt) * PLAYER_HURT_VERTICAL_DISTANCE / info_contentScaleFactor;
	}
	_body->setVelocity(_vecMove);
}

// index range: 2-3
void Player::startCharge(int index)
{
	MessageBox("This startCharge is obsolete!", "Warning");
	if (!isReadys[index])
		return;

	if (_chargeFX)
		return;

	_chargeFX = MAKE_EFFECT(Effect::CHARGE_GOKU
		, 0//x
		, 10 / info_contentScaleFactor//y
		, 1//direct
		, 0//repeat times
		, 0.5f);
	this->addChild(_chargeFX, -1);
}


void Player::startCharge()
{
	if (_chargeFX)
		return;

	_chargeFX = MAKE_EFFECT(Effect::CHARGE_GOKU
		, 0//x
		, 10 / info_contentScaleFactor//y
		, 1//direct
		, 0//repeat times
		, 0.5f);
	this->addChild(_chargeFX, -1);
}

void Player::stopCharge()
{
	if (_chargeFX)
	{
		this->removeChild(_chargeFX);
		_chargeFX = nullptr;
	}
}

float Player::getCooldownMax(int index)
{
	return maxCooldowns[index];
}

float Player::getCooldownRemain(int index)
{
	return cooldowns[index];
}

bool Player::isAttack() { return _isAttack; }