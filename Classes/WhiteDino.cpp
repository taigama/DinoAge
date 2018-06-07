#include "WhiteDino.h"

#include "Projectile.h"
#include "MyPoolManager.h"
#include "CameraHelper.h"

#include "WorldScene.h"
#include "PlayScene.h"


USING_NS_CC;



#define PLAYER_MAX_HEALTH 100
#define PLAYER_SPEED 200.0f
#define PLAYER_JUMP_SPEED 400.0f

#define PLAYER_ATTACK_TIME 0.4f

#define PLAYER_DENSITY 1.0f
#define PLAYER_FRICTION 0.0f


#define PLAYER_FORCE_FACTOR 0.8f


// position & scale
#define PLAYER_SCALE 1.2f
#define OFFSET_MOUTH_X 25.0f
#define OFFSET_MOUTH_Y -5.0f

////////////////////////////////////
#define PLAYER_HURT_TIME 0.3f
#define PLAYER_HURT_VERTICAL_DISTANCE 350.0f
#define PLAYER_HURT_DIAGONAL_DISTANCE 150.0f

#define PLAYER_DIED_TIME 1.0f

#define PLAYER_MAX_COOLDOWN_0 0.25f
#define PLAYER_MAX_COOLDOWN_1 0.5f
#define PLAYER_MAX_COOLDOWN_2 5.0f
#define PLAYER_MAX_COOLDOWN_3 5.0f

/////////////////////////


WhiteDino::~WhiteDino()
{
	//CC_SAFE_DELETE(_weapon);
	_weapon->release();
}

Rect WhiteDino::getBoundingBox() const
{
	Rect result = _sprite->getBoundingBox();

	// because this sprite is so small compared with the bounding box of the sprite
	result.size.width /= 2.0f;
	result.size.height /= 3.0f;

	return result;
}



bool WhiteDino::init()
{
	// must call before Player::init()
	_maxHP = PLAYER_MAX_HEALTH;

	if (!Player::init())
		return false;

	_direction = DIRECTION::RIGHT;
	
	// fixed, now he is white dinosaur :v
	_charType = CHARACTER_TYPE::WHITE_DINOSAUR;

	// speed based on the world's scale
	_speed = PLAYER_SPEED / info_contentScaleFactor;
	_jumpSpeed = PLAYER_JUMP_SPEED / info_contentScaleFactor;


	// attach the Jaw of this player to this player
	_weapon = Projectile::createProjectile(Projectile::PROJECTILE_TYPE::HORN_ATTACK, this);
	_weapon->retain();// 
	_weapon->setActive(false);

	// power of this player (pushing blocks), must <= 1
	_forceFactor = PLAYER_FORCE_FACTOR;

	//m_isRunAttack = false;



	m_offsetMouth = Vec2(OFFSET_MOUTH_X / info_contentScaleFactor, OFFSET_MOUTH_Y / info_contentScaleFactor);


	castSkill[0] = (CastSkill)(&WhiteDino::mysticDragon);
	castSkill[1] = (CastSkill)(&WhiteDino::iceWall);
	castSkill[2] = (CastSkill)(&WhiteDino::glacialBlade);

	return true;
}

// load sprites and animation, called in GameObject::init()
void WhiteDino::initSprites()
{

	//////////////////////////////
	_sprite = Sprite::createWithSpriteFrameName("trino_idle0.png");
	//_sprite->setPosition(Vec2(getBoundingBox().size.width / 6.0f, 0));
	_sprite->setScale(PLAYER_SCALE);
	this->addChild(_sprite, 0);

	/////////////////////////////////////////////
	// ANIM
	//////////////////////////
	char str[100] = { 0 };
	Vector<SpriteFrame*> frames;
	SpriteFrame* frame;
	Animation* anim;

	// ----------- idle --------------------
	for (int i = 0; i < 6; i++)
	{
		sprintf(str, "trino_idle%d.png", i);

		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim = Animation::createWithSpriteFrames(frames, 0.25f);
	act_idle = RepeatForever::create(Animate::create(anim));
	act_idle->retain();

	// ------------ walk --------------------
	frames.clear();
	for (int i = 0; i < 12; i++)
	{
		sprintf(str, "trino_walk%d.png", i);

		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	//frames.erase(6);
	//frames.erase(0);
	/*for (int i = 10; i < 12; i++)
	{
		sprintf(str, "trino_walk%d.png", i);

		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		walkFrames.pushBack(frame);
	}*/

	anim = Animation::createWithSpriteFrames(frames, 0.033f);
	act_walk = RepeatForever::create(Animate::create(anim));
	act_walk->retain();


	// ------------- attack ------------------
	frames.clear();
	for (int i = 1; i < 7; i++)
	{
		sprintf(str, "trino_attack%d.png", i);
		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}
	/*for (int i = 6; i > 0; i--)
	{
		sprintf(str, "trino_attack%d.png", i);
		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}*/
	//frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("trino_idle0.png");
	//frames.pushBack(frame);

	anim = Animation::createWithSpriteFrames(frames, 0.05f);
	act_attack = RepeatForever::create(Animate::create(anim));
	act_attack->retain();

	// ------------- attack 2 ------------------
	frames.clear();
	for (int i = 3; i < 7; i++)
	{
		sprintf(str, "trino_attack%d.png", i);
		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}
	frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("trino_idle0.png");
	frames.pushBack(frame);

	anim = Animation::createWithSpriteFrames(frames, 0.1f);
	act_attack2 = Animate::create(anim);
	act_attack2->retain();

	// ------------- hurt ------------------
	frames.clear();
	for (int i = 0; i < 3; i++)
	{
		sprintf(str, "trino_hurt%d.png", i);
		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim = Animation::createWithSpriteFrames(frames, 0.1f);
	act_hurt = Animate::create(anim);
	act_hurt->retain();
	
	// ------------- died ------------------
	frames.clear();
	for (int i = 0; i < 4; i++)
	{
		sprintf(str, "trino_die%d.png", i);
		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim = Animation::createWithSpriteFrames(frames, 0.1f);
	act_die = Animate::create(anim);
	act_die->retain();















	// ------------ jump ---------------- (not complete)
	frames.clear();/*
	frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("trino_walk0.png");
	frames.pushBack(frame);*/
	frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("trino_walk1.png");
	frames.pushBack(frame);
	frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("trino_walk2.png");
	frames.pushBack(frame);
	//frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("trino_walk0.png");
	//frames.pushBack(frame);
	//frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("trino_walk7.png");
	//frames.pushBack(frame);

	anim = Animation::createWithSpriteFrames(frames, 0.1f);
	act_jump = Animate::create(anim);
	act_jump->retain();



















	
	///////////////////////////////////////
	// start animation
	
	_action = act_idle;
	_sprite->runAction(_action);
}

void WhiteDino::initPhysics()
{
	// ===============================================================
	_body = PhysicsBody::createBox(getBoundingBox().size, PhysicsMaterial(PLAYER_DENSITY, 0.0f, PLAYER_FRICTION));
	_body->setPositionOffset( Vec2(0, - getBoundingBox().size.height / 5.0f));

	_body->setDynamic(true);
	_body->setRotationEnable(false);

	this->setPhysicsBody(_body);


	setColliderBitMask();
}




/* MAIN UPDATE FUNCTION OF PLAYER
* should not read this now :v */
void WhiteDino::update(float delta)
{
	// ### GET VELOCITY FIRST! ###
	_vecMove = _body->getVelocity();

	// update camera based on this position
	//updateCamera();
	CameraHelper::getInstance()->updatePosition(_position);

	// update cooldowns
	updateCooldowns(delta);

	


	// hurted? player can't attack, move or do anything else
	if (_state == STATE::HURTED)
	{
		_timerHurt -= delta;

		if (_timerHurt <= 0)
		{
			// now I'm not hurted anymore, I continue to kill this world

			if (_hp <= 0)
			{// or I restart the game :v
				//onDie();
				AUDIO::play2d("lifelost.ogg");

				// GAME OVER
				auto currentScene = dynamic_cast<Layer*>(Director::getInstance()->getRunningScene()->getChildByName("play_scene"));
				auto playScene = (PlayScene*)(currentScene);

				playScene->gameOver();


				return;
			}

			_state = STATE::DOUBLE_JUMP;
		}
		else
		{
			return;
		}
	}
	
	
	// not idle? => now we are moving
	if(_state != STATE::IDLE)
	{
		_vecMove.x = _speed * _moveDirect;	
	}
	else // now stop
	{
		_vecMove.x = 0;
	}

	// Detached from ground, I'm falling!
	if (_state == STATE::IDLE || _state == STATE::RUN)
	{		
		//if (_vecMove.y < -1.0f)
		if (!_hasGround)
		{
			_state = STATE::JUMP;

			if (_action == act_walk)
			{
				_sprite->stopAction(_action);

				_action = act_idle;
				_sprite->runAction(_action);
			}
		}
	}
	_hasGround = false;

	if (_isAttack)
	{// attack time!
		/*if (m_isRunAttack)
		{
			_vecMove.x = (int)_direction * _speed;
		}*/
		//_vecMove = Vec2::ZERO;

		_timerAttack -= delta;
		if (_timerAttack <= 0)
		{
			endAttack();
		}
	}


	// push this player go with moving block
	if (_movingBlockBody)
	{
		/*if (_state == STATE::IDLE)
		{
			_vecMove.x += _movingBlockBody->getVelocity().x;
		}
		else if (_state == STATE::RUN)
		{
			_vecMove.x += _movingBlockBody->getVelocity().x + _moveDirect * PLAYER_SPEED;
		}*/
		_vecMove.x += _movingBlockBody->getVelocity().x;
		_vecMove.y = _movingBlockBody->getVelocity().y;
		_movingBlockBody = nullptr;
	}


	


	
	handleOverlapBlock();


	// ### SET VELOCITY LAST! ###
	_body->setVelocity(_vecMove);
}


/* public method for make this character attack in melee */
void WhiteDino::attackPhysical()
{
	// check cooldown
	if (!isReadys[0])
		return;

	// already attack? no need to attack more
	/*if (m_isRunAttack)
		return;*/

	// you can't fire when hurted!
	if (_state == STATE::HURTED)// || _state == STATE::DOUBLE_JUMP || _state == STATE::JUMP)
		return;

	// stop movement when attacking
	if (_state == STATE::RUN)
	{
		endWalk();
	}
	
	// animation
	_sprite->stopAction(_action);

	_action = act_attack2;//act_attack2
	_sprite->runAction(_action);



	// ------------ head of white dinosaur --------------
	AUDIO::play2d("grr.ogg");

	if (_weapon->isActive())
	{
		_weapon->setActive(false);
	}

	// call physical projectile
	float i_a[10] = {
		getPosition().x + (int)_direction * m_offsetMouth.x,
		getPosition().y + m_offsetMouth.y,
		(int)(_direction) ,
		_team,
		0,0,0,0,0,0 };
	_weapon->reset(i_a);

	this->addChild(_weapon);

	//_weapon->scheduleUpdate();
	// because this weapon need update function

	_state = STATE::IDLE;




	_isAttack = true;
	//m_isRunAttack = true;

	_timerAttack = PLAYER_ATTACK_TIME;

	isReadys[0] = false;
	cooldowns[0] = PLAYER_MAX_COOLDOWN_0;
}

/* public method for make this character fire a projectile */
void WhiteDino::attackProjectile()
{
	// check cooldown
	if (!isReadys[1])
		return;

	// already run attack? don't stop!
	/*if (m_isRunAttack)
		return;*/

	// you can't fire when hurted!
	if (_state == STATE::HURTED)
		return;


	// stop movement when attacking
	if (_state == STATE::RUN)
	{
		endWalk();
	}

	AUDIO::play2d("ONPARE8_00004.ogg");
	// animation
	_sprite->stopAction(_action);

	_action = act_attack2;
	_sprite->runAction(_action);



	SPAWN_PROJECTILE(Projectile::PROJECTILE_TYPE::ICE_SHARD,
		getPosition().x + (int)_direction * m_offsetMouth.x,
		getPosition().y + m_offsetMouth.y,
		(int)(_direction),
		_team,
		World::WORLD_LAYER::OBJECT);



	_isAttack = true;
	_timerAttack = PLAYER_ATTACK_TIME;

	isReadys[1] = false;
	cooldowns[1] = PLAYER_MAX_COOLDOWN_1;
}

void WhiteDino::endAttack()
{
	_isAttack = false;
	//m_isRunAttack = false;
	_weapon->setActive(false);

	if (_state == STATE::IDLE || _state == STATE::RUN)
	{
		if (_moveDirect == 0)
		{
			_sprite->stopAction(_action);
			_action = act_idle;
			_sprite->runAction(_action);
			_state = STATE::IDLE;
		}
		else
		{
			startWalk();
		}
	}
}

//
///* knock back
//* direction of player, knocking to
//# onHit() call this*/
//void WhiteDino::knock(DIRECTION dir, float force)
//{
//	_isAttack = false;
//	m_isRunAttack = false;
//
//	_state = STATE::HURTED;
//
//
//
//
//	_sprite->stopAction(_action);
//
//	if (_hp > 0)
//	{
//		_timerHurt = PLAYER_HURT_TIME;
//		_action = act_hurt;
//	}
//	else
//	{
//		_body->getShapes().at(0)->setFriction(10.0f);
//		_body->setContactTestBitmask(0);
//
//		_timerHurt = PLAYER_DIED_TIME;
//		_action = act_die;
//	}
//
//
//
//	_sprite->runAction(_action);
//
//
//	// from left or right
//	if (dir != DIRECTION::BOTTOM)
//	{
//		_vecMove.x = force * (-(int)dir) * PLAYER_HURT_DIAGONAL_DISTANCE / info_contentScaleFactor;
//		_vecMove.y = force * PLAYER_HURT_DIAGONAL_DISTANCE / info_contentScaleFactor;
//
//		this->setScaleX((int)dir);
//		_direction = (DIRECTION)dir;
//	}
//	else
//	{
//		_vecMove.x = 0;
//		_vecMove.y = force * PLAYER_HURT_VERTICAL_DISTANCE / info_contentScaleFactor;
//	}
//	_body->setVelocity(_vecMove);
//}

void WhiteDino::mysticDragon()
{
	// check cooldown
	if (!isReadys[2])
		return;

	// already run attack? don't stop!
	/*if (m_isRunAttack)
		return;*/

	// you can't fire when hurted!
	if (_state == STATE::HURTED)
		return;


	// stop movement when attacking
	if (_state == STATE::RUN)
	{
		endWalk();
	}

	AUDIO::play2d("ST07_01_00000.ogg");

	// animation
	_sprite->stopAction(_action);
	_action = act_attack2;
	_sprite->runAction(_action);
	

	if (_chargeFX)
	{
		this->removeChild(_chargeFX);
		_chargeFX = nullptr;
	}

	_isAttack = true;
	_timerAttack = PLAYER_ATTACK_TIME;



	SPAWN_PROJECTILE(Projectile::PROJECTILE_TYPE::ICE_DRAGON,
		getPosition().x + (int)_direction * m_offsetMouth.x * 3.0f,
		getPosition().y - m_offsetMouth.y * 5.0f,
		(int)(_direction),
		_team,
		World::WORLD_LAYER::OBJECT);

	isReadys[2] = false;
	cooldowns[2] = PLAYER_MAX_COOLDOWN_2;
}

void WhiteDino::iceWall()
{
	// check cooldown
	if (!isReadys[3])
		return;

	//// already run attack? don't stop!
	//if (m_isRunAttack)
	//	return;

	// you can't fire when hurted!
	if (_state == STATE::HURTED)
		return;


	// stop movement when attacking
	if (_state == STATE::RUN)
	{
		endWalk();
	}


	AUDIO::play2d("ice_wall.ogg");

	// animation
	_sprite->stopAction(_action);
	_action = act_attack2;
	_sprite->runAction(_action);


	if (_chargeFX)
	{
		this->removeChild(_chargeFX);
		_chargeFX = nullptr;
	}


	_isAttack = true;
	_timerAttack = PLAYER_ATTACK_TIME;

	float scale_factor = Director::getInstance()->getContentScaleFactor();
	auto physics_world = Director::getInstance()->getRunningScene()->getPhysicsWorld();

	Vec2 myPos = getPosition();
	Vec2 startPos = Vec2(myPos.x - m_offsetMouth.x * 3.0f, myPos.y);
	Vec2 endPos = Vec2(startPos.x, startPos.y - 300.0f / scale_factor);

	m_countWallLeft = 0;
	physics_world->rayCast(
		CC_CALLBACK_3(WhiteDino::wallRaycastCallback, this), startPos, endPos, &m_countWallLeft);


	startPos.x = myPos.x + m_offsetMouth.x * 3.0f;
	endPos.x = startPos.x;

	m_countWallRight = 0;
	physics_world->rayCast(
		CC_CALLBACK_3(WhiteDino::wallRaycastCallback, this), startPos, endPos, &m_countWallRight);



	isReadys[3] = false;
	cooldowns[3] = PLAYER_MAX_COOLDOWN_3;
}

// callback to make ice wall
bool WhiteDino::wallRaycastCallback(cocos2d::PhysicsWorld& world, const cocos2d::PhysicsRayCastInfo& info, void* data)
{
	// m_countWall...
	if (*((int*)data) > 0)
		return false;


	auto contactBody = info.shape->getBody();
	if (contactBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::BLOCK)
	{
		// prevent walls upon walls
		GameObject* owner = dynamic_cast<GameObject*>(contactBody->getOwner()->getParent());
		if (owner != nullptr)
		{
			// if the block that has parent is a GameObject
			return false;
		}//-----------

		if (info.contact.x < this->getPositionX())
		{
			SPAWN_PROJECTILE(Projectile::PROJECTILE_TYPE::ICE_WALL,
				info.contact.x, // X
				info.contact.y + (50.0f / Director::getInstance()->getContentScaleFactor()), // Y
				-1, // direction
				_team,
				World::WORLD_LAYER::OBJECT);
			m_countWallLeft++;
		}
		else
		{
			SPAWN_PROJECTILE(Projectile::PROJECTILE_TYPE::ICE_WALL,
				info.contact.x, // X
				info.contact.y + (50.0f / Director::getInstance()->getContentScaleFactor()), // Y
				1, // direction
				_team,
				World::WORLD_LAYER::OBJECT);
			m_countWallRight++;
		}		
	}

	return true;
}


void WhiteDino::glacialBlade()
{
	// you can't fire when hurted!
	if (_state == STATE::HURTED)
		return;


	// stop movement when attacking
	if (_state == STATE::RUN)
	{
		endWalk();
	}


	// animation
	_sprite->stopAction(_action);
	_action = act_attack;
	_sprite->runAction(_action);


	SPAWN_EFFECT(Effect::FLASH_ULTI
		, _position.x
		, _position.y
		, 1
		, 1
		, 1 // scale
		, World::WORLD_LAYER::EFFECT_FRONT);

	SPAWN_DUMMY(Dummy::DUMMY_TYPE::CAST_GLACIAL_BLADE
		, getPosition().x// + (int)_direction * m_offsetMouth_fire.x + 200
		, getPosition().y// + m_offsetMouth_fire.y
		, (int)_direction
		, _team
		, 0 // test
		, 0.1//delay per cast
		, 50// number of cast
		, 500//width of the area
		, World::WORLD_LAYER::OBJECT);


	_isAttack = true;
	_timerAttack = PLAYER_ATTACK_TIME;
}