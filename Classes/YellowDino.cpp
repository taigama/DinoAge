#include "YellowDino.h"

#include "Projectile.h"
#include "MyPoolManager.h"

#include "WorldScene.h"

#include "Dummy.h"

#include "CameraHelper.h"

#include "TextEffect.h"

#include "PlayScene.h"



USING_NS_CC;



#define PLAYER_MAX_HEALTH 100
#define PLAYER_SPEED 200.0f
#define PLAYER_JUMP_SPEED 400.0f

#define PLAYER_ATTACK_TIME 0.4f

#define PLAYER_DENSITY 1.0f
#define PLAYER_FRICTION 0.0f


#define PLAYER_FORCE_FACTOR 0.8f

#define SKILL_NOVA_DELAY 0.9f


#define PLAYER_MAX_COOLDOWN_0 0.25f
#define PLAYER_MAX_COOLDOWN_1 1.5f
#define PLAYER_MAX_COOLDOWN_2 5.0f
#define PLAYER_MAX_COOLDOWN_3 5.0f




#define SPRITE_OFFSET_Y 7.0f
#define PLAYER_SCALE 0.8f

YellowDino::~YellowDino()
{
	//CC_SAFE_DELETE(_weapon);
	_weapon->release();

	//CC_SAFE_DELETE(act_nova);
	//act_nova->release();
}

Rect YellowDino::getBoundingBox() const
{
	Rect result = _sprite->getBoundingBox();

	// because this sprite is so small compared with the bounding box of the sprite
	result.size.width /= 3.0f;
	result.size.height /= 1.5f;

	return result;
}



bool YellowDino::init()
{
	// must call before Player::init()
	_maxHP = PLAYER_MAX_HEALTH;

	if (!Player::init())
		return false;
	_direction = DIRECTION::RIGHT;
	
	// yes, this is Red Dinosaur
	_charType = CHARACTER_TYPE::YELLOW_DINOSAUR;

	// speed based on the world's scale
	_speed = PLAYER_SPEED / info_contentScaleFactor;
	_jumpSpeed = PLAYER_JUMP_SPEED / info_contentScaleFactor;


	// attach the Jaw of this player to this player
	_weapon = Projectile::createProjectile(Projectile::PROJECTILE_TYPE::YCLAW_ATTACK, this);
	_weapon->retain();// keep this object remains after removed from it's father node
	_weapon->setActive(false);

	// power of this player (pushing blocks), must <= 1
	_forceFactor = PLAYER_FORCE_FACTOR;



	// offset for attacking
	m_offsetMouth_bite = Vec2(this->getBoundingBox().size.width, 0);
	m_offsetMouth_fire = Vec2((this->getBoundingBox().size.width) / 1.5f, 0.0f);


	castSkill[0] = (CastSkill)(&YellowDino::lightningStrike);
	castSkill[1] = (CastSkill)(&YellowDino::thunderBody);
	castSkill[2] = (CastSkill)(&YellowDino::dragonNova);

	//act_nova = Sequence::createWithTwoActions(
	//	DelayTime::create(SKILL_NOVA_DELAY),
	//	CallFunc::create(
	//		[this]()
	//		{
	//			Vec2 tmpPos = CameraHelper::getInstance()->getPosition();

	//			AUDIO::play2d("fire_crash.ogg");
	//			SPAWN_PROJECTILE(Projectile::PROJECTILE_TYPE::DRAGON_NOVA
	//				//, _camera->getPositionX()
	//				, tmpPos.x
	//				//, _camera->getPositionY()
	//				, tmpPos.y
	//				, 1
	//				, this->getTeam()
	//				, World::WORLD_LAYER::OBJECT);
	//		}
	//	)
	//);
	//act_nova->retain();

	return true;
}

// load sprites and animation, called in GameObject::init()
void YellowDino::initSprites()
{

	//////////////////////////////
	_sprite = Sprite::createWithSpriteFrameName("yellow_dino_stand0.png");
	_sprite->setPosition(0, (SPRITE_OFFSET_Y * PLAYER_SCALE) / info_contentScaleFactor);
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
	for (int i = 0; i < 3; i++)
	{
		sprintf(str, "yellow_dino_stand%d.png", i);

		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim = Animation::createWithSpriteFrames(frames, 0.15f);
	act_idle = RepeatForever::create(Animate::create(anim));
	act_idle->retain();

	// ------------ walk --------------------
	frames.clear();
	for (int i = 0; i < 5; i++)
	{
		sprintf(str, "yellow_dino_walk%d.png", i);

		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	/*frames.erase(6);
	frames.erase(0);*/
	/*for (int i = 10; i < 12; i++)
	{
		sprintf(str, "tyro_walk%d.png", i);

		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		walkFrames.pushBack(frame);
	}*/

	anim = Animation::createWithSpriteFrames(frames, 0.07f);
	act_walk = RepeatForever::create(Animate::create(anim));
	act_walk->retain();


	// ------------- attack ------------------
	frames.clear();
	//for (int i = 2; i < 3; i++)
	//{
		//sprintf(str, "yellow_dino_kick%d.png", i);
		//frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		//frames.pushBack(frame);
	//}
	//frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("yellow_dino_stand0.png");
	//frames.pushBack(frame);

	frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("yellow_dino_kick2.png");
	frames.pushBack(frame);
	frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("yellow_dino_kick3.png");
	frames.pushBack(frame);
	frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("yellow_dino_kick3.png");
	frames.pushBack(frame);
	frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("yellow_dino_kick4.png");
	frames.pushBack(frame);
	frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("yellow_dino_stand0.png");
	frames.pushBack(frame);

	anim = Animation::createWithSpriteFrames(frames, 0.12f);
	act_attack = Animate::create(anim);
	act_attack->retain();

	// ------------- attack 2 ------------------
	frames.clear();
	for (int i = 2; i < 4; i++)
	{
		sprintf(str, "yellow_dino_headup%d.png", i);
		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}
	frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("yellow_dino_stand0.png");
	frames.pushBack(frame);

	anim = Animation::createWithSpriteFrames(frames, 0.12f);
	act_attack2 = Animate::create(anim);
	act_attack2->retain();

	// ------------- hurt ------------------
	frames.clear();
	for (int i = 0; i < 2; i++)
	{
		sprintf(str, "yellow_dino_hurt%d.png", i);
		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim = Animation::createWithSpriteFrames(frames, 0.1f);
	act_hurt = Animate::create(anim);
	act_hurt->retain();
	
	//// ------------- died ------------------
	//frames.clear();
	//for (int i = 0; i < 5; i++)
	//{
	//	sprintf(str, "tyro_die%d.png", i);
	//	frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
	//	frames.pushBack(frame);
	//}

	//anim = Animation::createWithSpriteFrames(frames, 0.1f);
	//act_die = Animate::create(anim);
	act_die = act_hurt->clone();
	act_die->retain();















	// ------------ jump ---------------- (not complete)
	frames.clear();/*
	frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("tyro_walk0.png");
	frames.pushBack(frame);*/
	frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("yellow_dino_slam4.png");
	frames.pushBack(frame);
	frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("yellow_dino_slam3.png");
	frames.pushBack(frame);
	//frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("tyro_walk0.png");
	//frames.pushBack(frame);
	//frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("tyro_walk7.png");
	//frames.pushBack(frame);

	anim = Animation::createWithSpriteFrames(frames, 0.1f);
	act_jump = Animate::create(anim);
	act_jump->retain();



















	
	///////////////////////////////////////
	// start animation
	
	_action = act_idle;
	_sprite->runAction(_action);
}

void YellowDino::initPhysics()
{
	// ===============================================================
	_body = PhysicsBody::createBox(getBoundingBox().size, PhysicsMaterial(PLAYER_DENSITY, 0.0f, PLAYER_FRICTION));

	_body->setDynamic(true);
	_body->setRotationEnable(false);

	this->setPhysicsBody(_body);


	setColliderBitMask();
}




/* MAIN UPDATE FUNCTION OF PLAYER
* should not read this now :v */
void YellowDino::update(float delta)
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
				AUDIO::play2d("lifelost.ogg");
				//onDie();

				// GAME OVER
				auto currentScene = dynamic_cast<Layer*>(Director::getInstance()->getRunningScene()->getChildByName("play_scene"));
				auto playScene = (PlayScene*)(currentScene);

				playScene->gameOver();


				return;
			}


			// change _state
			//if (_vecMove.y < -1.0f)
			//{
			//	_state = STATE::DOUBLE_JUMP;
			//}
			//else // standing in the ground
			//{
			//	if (_moveDirect == 0)
			//	{
			//		endWalk();
			//	}
			//	else
			//	{
			//		startWalk();
			//	}
			//}
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
		if(!_hasGround)
		//if (_vecMove.y < -1.0f)
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


	/*if (_moveDirect != 0
		&& (_vecMove.x > -_speed && _vecMove.x < _speed)
		&& (_state == STATE::JUMP || _state == STATE::DOUBLE_JUMP || _isAttack == false))
	{
		_vecMove.x = _speed* _moveDirect;
		
	}*/
	

	if (_isAttack)
	{// attack time!
		_timerAttack -= delta;
		//_vecMove = Vec2::ZERO;
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
			_vecMove = _movingBlockBody->getVelocity();
		}
		else if (_state == STATE::RUN)
		{
			_vecMove.x = _movingBlockBody->getVelocity().x + _moveDirect * PLAYER_SPEED;
			_vecMove.y = _movingBlockBody->getVelocity().y;
		}*/
		_vecMove.x += _movingBlockBody->getVelocity().x;
		_vecMove.y = _movingBlockBody->getVelocity().y;
		_movingBlockBody = nullptr;

		//_position.y -= 0.5f;
	}


	


	
	handleOverlapBlock();


	// ### SET VELOCITY LAST! ###
	_body->setVelocity(_vecMove);
}


/* public method for make this character attack in melee */
void YellowDino::attackPhysical()
{
	// check cooldown
	if (!isReadys[0])
		return;

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




	if (_weapon->isActive())
	{
		_weapon->setActive(false);
	}

	// call physical projectile
	float i_a[10] = {
		getPosition().x + (((int)_direction) * m_offsetMouth_bite.x),
		getPosition().y + m_offsetMouth_bite.y,
		(int)(_direction) ,
		_team,
		0,0,0,0,0,0 };
	_weapon->reset(i_a);

	this->addChild(_weapon);

	_weapon->scheduleUpdate();
	// because this weapon need update function


	// effect for this attack
	/*auto fx = MAKE_EFFECT(Effect::BITE,
		getPosition().x + (((int)this->_direction) * m_offsetMouth_bite.x),
		getPosition().y + m_offsetMouth_bite.y,
		((int)_direction),
		1,
		1);
	World::getInstance()->addChild(fx, World::WORLD_LAYER::EFFECT_FRONT);*/
	AUDIO::play2d("ST01_00_00002.ogg");

	SPAWN_EFFECT(Effect::BITE,
		getPosition().x + (((int)this->_direction) * m_offsetMouth_bite.x),
		getPosition().y + m_offsetMouth_bite.y,
		((int)_direction),
		1,// repeat times
		1,// scale
		World::WORLD_LAYER::EFFECT_FRONT);



	_isAttack = true;
	_timerAttack = PLAYER_ATTACK_TIME;

	isReadys[0] = false;
	cooldowns[0] = PLAYER_MAX_COOLDOWN_0;
}

/* public method for make this character fire a projectile */
void YellowDino::attackProjectile()
{
	// check cooldown
	if (!isReadys[1])
		return;

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

	AUDIO::play2d("ST01_00_00001.ogg");
	SPAWN_PROJECTILE(Projectile::PROJECTILE_TYPE::ELECT_ORB,
		getPosition().x + (int)_direction * m_offsetMouth_fire.x,//tmp
		getPosition().y + m_offsetMouth_fire.y,
		(int)(_direction),
		_team,
		World::WORLD_LAYER::OBJECT);


	_isAttack = true;
	_timerAttack = PLAYER_ATTACK_TIME;

	isReadys[1] = false;
	cooldowns[1] = PLAYER_MAX_COOLDOWN_1;

	/*auto speech = TextEffect::createWithTTF("Baby Flame!", "gamecuben.ttf", 20, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);

	float params[10] = { _position.x
		, _position.y
		, 0
		, TextEffect::PATH_TYPE::FLY_UP
		, 0
		, 0.5f
		, 255, 50, 50, 255};

	speech->reset(params);
	World::getCurrent()->addChild((Label*)speech, World::WORLD_LAYER::EFFECT_BACK);*/
}

void YellowDino::thunderBody()
{
	// check cooldown
	if (!isReadys[3])
		return;

	// you can't fire when hurted!
	if (_state == STATE::HURTED)
		return;


	// stop movement when attacking
	if (_state == STATE::RUN)
	{
		endWalk();
	}


	if (_chargeFX)
	{
		this->removeChild(_chargeFX);
		_chargeFX = nullptr;
	}

	// animation
	_sprite->stopAction(_action);
	_action = act_attack2;
	_sprite->runAction(_action);

	AUDIO::play2d("electric.ogg");
	
	Dummy* dummy = MAKE_DUMMY(Dummy::DUMMY_TYPE::CAST_THUNDER_BODY
		, 0//getPosition().x// + (int)_direction * m_offsetMouth_fire.x + 200
		, 0//getPosition().y// + m_offsetMouth_fire.y
		, 1//(int)_direction
		, _team
		, 0 // speed
		, 0.2//delay per cast
		, 5// number of cast
		, 0//width of the area
	);
	//World::getCurrent()->addChild(dummy);
	this->addChild(dummy);
	dummy->setHost(this);
	


	_isAttack = true;
	_timerAttack = PLAYER_ATTACK_TIME;

	isReadys[3] = false;
	cooldowns[3] = PLAYER_MAX_COOLDOWN_3;
}

void YellowDino::lightningStrike()
{
	// check cooldown
	if (!isReadys[2])
		return;

	// you can't fire when hurted!
	if (_state == STATE::HURTED)
		return;


	// stop movement when attacking
	if (_state == STATE::RUN)
	{
		endWalk();
	}


	if (_chargeFX)
	{
		this->removeChild(_chargeFX);
		_chargeFX = nullptr;
	}

	// animation
	_sprite->stopAction(_action);
	_action = act_attack2;
	_sprite->runAction(_action);


	//// ############################# test #####################################
	//// test fx
	//SPAWN_EFFECT(Effect::BLACK_IN
	//	, _position.x
	//	, _position.y
	//	, 1
	//	, 1
	//	, 1 // scale
	//	, World::WORLD_LAYER::EFFECT_FRONT);


	//// test dark portal
	//auto dark_portal = Character::createCharacter(CHARACTER_TYPE::DARK_PORTAL, 2);
	//dark_portal->setPosition(_position.x, _position.y);
	//this->getParent()->addChild(dark_portal, World::WORLD_LAYER::GROUND);


	//// ####################################################################################


	SPAWN_DUMMY(Dummy::DUMMY_TYPE::CAST_LIGHTNING_STRIKE
		, getPosition().x// + (int)_direction * m_offsetMouth_fire.x + 200
		, getPosition().y// + m_offsetMouth_fire.y
		, (int)_direction
		, _team
		, 300 // speed
		, 0.1//delay per cast
		, 18// number of cast
		, 0//width of the area
		, World::WORLD_LAYER::OBJECT);


	_isAttack = true;
	_timerAttack = PLAYER_ATTACK_TIME;

	isReadys[2] = false;
	cooldowns[2] = PLAYER_MAX_COOLDOWN_2;
}

void YellowDino::dragonNova()
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
	_action = act_attack2;
	_sprite->runAction(_action);


	SPAWN_EFFECT(Effect::EFFECT_TYPE::FLASH_ULTI
		, _position.x
		, _position.y
		, 1
		, 1
		, 1 // scale
		, World::WORLD_LAYER::EFFECT_FRONT);


	Vec2 tmpPos = CameraHelper::getInstance()->getPosition();

	//AUDIO::play2d("fire_crash.ogg");
	SPAWN_EFFECT(Effect::EFFECT_TYPE::THUNDER_HEAD
		, tmpPos.x
		, tmpPos.y
		, 1
		, 1
		, 1 // scale
		, World::WORLD_LAYER::EFFECT_FRONT);

	SPAWN_PROJECTILE(Projectile::PROJECTILE_TYPE::ZEUS_WRATH
		, tmpPos.x
		, tmpPos.y
		, 1
		, this->getTeam()
		, World::WORLD_LAYER::OBJECT);

	_isAttack = true;
	_timerAttack = PLAYER_ATTACK_TIME;
}