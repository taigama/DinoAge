#include "IceShard.h"


#include "MyPoolManager.h"
#include "WorldScene.h"

#include "AudioEngine.h"



USING_NS_CC;



#define SHARD_DAMAGE 5
#define SHARD_SPEED 300

IceShard::~IceShard() {}

bool IceShard::init()
{
	if (!Projectile::init())
		return false;
	// sprite init & physic init were called in base

	_projectileType = PROJECTILE_TYPE::ICE_SHARD;

	_speed = SHARD_SPEED / Director::getInstance()->getContentScaleFactor();
	_damage = SHARD_DAMAGE;

	_isStatic = false;
	
	return true;
}

void IceShard::update(float delta)
{
	// go outside the screen? become in-active

	Size visibleSize = Director::getInstance()->getVisibleSize();
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
	}
}






void IceShard::initSprites()
{
	float scale_factor = Director::getInstance()->getContentScaleFactor();

	_sprite = Sprite::create();
	_sprite->setPosition(- 10.0f / scale_factor, 0);
	this->addChild(_sprite);









	Vector<SpriteFrame*> frames;

	char str[100] = { 0 };
	SpriteFrame* frame;
	for (int i = 0; i <= 3; i++)
	{
		sprintf(str, "ice_shard_%d.png", i);
		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	auto anim = Animation::createWithSpriteFrames(frames, 0.05f);

	act_animate = RepeatForever::create(Animate::create(anim));
	act_animate->retain();
}

void IceShard::initPhysics()
{
	float scale_factor = Director::getInstance()->getContentScaleFactor();

	Size s = Size(12.0f / scale_factor, 12.0f / scale_factor);

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
void IceShard::reset(float params[10])
{
	this->setActive(true);

	this->setPosition(Vec2(params[0], params[1]));
	this->setScaleX(params[2]);

	scheduleUpdate();

	// Unique for Fire projectile
	_body->setVelocity(Vec2(params[2] * _speed, 0));	
	//this->runAction(RepeatForever::create(RotateBy::create(0.5f, Vec3(0, 0, -360.0f * params[2]))));
	_sprite->runAction(act_animate);

	_team = params[3];
}

void IceShard::attack(Character* target, Character::DIRECTION dir)
{
	// make enemy become hurted
	Projectile::attack(target, dir);

	onDie();
}

void IceShard::onDie()
{
	SPAWN_EFFECT(Effect::EFFECT_TYPE::ICE_BALL_CRASH, this->getPositionX(), this->getPositionY(), 1, 1, 1, World::WORLD_LAYER::EFFECT_FRONT);
	experimental::AudioEngine::play2d("ice_crash.ogg");

	this->setActive(false);
}