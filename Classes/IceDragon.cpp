#include "IceDragon.h"



#include "MyPoolManager.h"
//#include "Effect.h"
#include "WorldScene.h"



USING_NS_CC;



#define DRAGON_DAMAGE 25
#define DRAGON_SPEED 400

#define DRAGON_LIFE_TIME 0.5f

IceDragon::~IceDragon() {}

bool IceDragon::init()
{
	if (!Projectile::init())
		return false;
	// sprite init & physic init were called in base

	_projectileType = PROJECTILE_TYPE::ICE_DRAGON;

	_speed = DRAGON_SPEED / Director::getInstance()->getContentScaleFactor();
	_damage = DRAGON_DAMAGE;


	
	return true;
}

void IceDragon::update(float delta)
{
	// go outside the screen? become in-active

	/*Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 camPos = Camera::getDefaultCamera()->getPosition();

	Vec2 myPos = this->getPosition();

	if (
		(myPos.x > camPos.x + visibleSize.width / 2)
		|| (myPos.x < camPos.x - visibleSize.width / 2)
		|| (myPos.y > camPos.y + visibleSize.height / 2)
		|| (myPos.y < camPos.y - visibleSize.height / 2)
		)
	{
		this->setActive(false);
	}*/
	/*if (act_animate->isDone())
	{
		this->setActive(false);
	}*/
	(this->*doNow)(delta);
}

void IceDragon::doNew(float delta)
{
	if (act_animateNew->isDone())
	{
		doNow = &IceDragon::doGo;
		_sprite->runAction(act_animateGo);
		_body->setVelocity(Vec2(m_direction * _speed, 0));
	}
}

void IceDragon::doGo(float delta)
{
	timer -= delta;
	if (timer <= 0)
	{
		doNow = &IceDragon::doEnd;
		_sprite->runAction(act_animateEnd);
		//this->setActive(false);
		//_body->setVelocity(Vec2::ZERO);
		_body->setVelocity(Vec2(m_direction * _speed, 0));
	}
}

void IceDragon::doEnd(float delta)
{
	if (act_animateEnd->isDone())
	{
		this->setActive(false);
	}
}




void IceDragon::initSprites()
{
	float scale_factor = Director::getInstance()->getContentScaleFactor();

	_sprite = Sprite::create();
	this->addChild(_sprite);









	Vector<SpriteFrame*> frames;

	char str[100] = { 0 };
	SpriteFrame* frame;
	for (int i = 0; i <= 2; i++)
	{
		sprintf(str, "ice_dragon_%d.png", i);
		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}
	auto anim = Animation::createWithSpriteFrames(frames, 0.1f);
	act_animateNew = Animate::create(anim);
	act_animateNew->retain();
	frames.clear();

	for (int i = 3; i <= 4; i++)
	{
		sprintf(str, "ice_dragon_%d.png", i);
		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}
	anim = Animation::createWithSpriteFrames(frames, 0.1f);
//	act_animateGo = RepeatForever::create(Animate::create(anim));
	act_animateGo = Animate::create(anim);
	act_animateGo->retain();
	frames.clear();

	for (int i = 5; i <= 9; i++)
	{
		sprintf(str, "ice_dragon_%d.png", i);
		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}
	anim = Animation::createWithSpriteFrames(frames, 0.1f);
	act_animateEnd = Animate::create(anim);
	act_animateEnd->retain();
}

void IceDragon::initPhysics()
{
	float scale_factor = Director::getInstance()->getContentScaleFactor();

	Size s = Size(90.0f / scale_factor, 90.0f / scale_factor);

	_body = PhysicsBody::createBox(s, PhysicsMaterial(0.1F, 0.0f, 0.0f));
	_body->setDynamic(true);
	_body->setGravityEnable(false);
	this->setPhysicsBody(_body);

	// call Projectile's func
	this->setColliderBitMask();
}



/* Reset this object
parameter: x, y, direct, team
// method called by pool*/
void IceDragon::reset(float params[10])
{
	this->setActive(true);
	scheduleUpdate();
	doNow = &IceDragon::doNew;
	timer = DRAGON_LIFE_TIME;

	this->setPosition(Vec2(params[0], params[1]));
	this->setScaleX(params[2]);
	m_direction = params[2];

	_body->setVelocity(Vec2::ZERO);
	_sprite->runAction(act_animateNew);

	_team = params[3];

	
}