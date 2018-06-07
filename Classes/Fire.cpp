#include "Fire.h"



#include "MyPoolManager.h"
//#include "Effect.h"
#include "WorldScene.h"


USING_NS_CC;



#define FIRE_DAMAGE 5
#define FIRE_SPEED 300


#define SIZE_WIDTH 12.0f

#define SPRITE_OFFSET_X -10.0f

Fire::~Fire() {}

bool Fire::init()
{
	if (!Projectile::init())
		return false;
	// sprite init & physic init were called in base

	_projectileType = PROJECTILE_TYPE::FIRE;

	_speed = FIRE_SPEED / Director::getInstance()->getContentScaleFactor();
	_damage = FIRE_DAMAGE;

	_isStatic = false;
	
	return true;
}

void Fire::update(float delta)
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






void Fire::initSprites()
{
	float scale_factor = Director::getInstance()->getContentScaleFactor();

	_sprite = Sprite::create();
	_sprite->setPosition(SPRITE_OFFSET_X / scale_factor, 0);
	this->addChild(_sprite);









	Vector<SpriteFrame*> frames;

	char str[100] = { 0 };
	SpriteFrame* frame;
	for (int i = 0; i <= 5; i++)
	{
		sprintf(str, "fire_ball/fire_yello_%d.png", i);
		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	auto anim = Animation::createWithSpriteFrames(frames, 0.08f);

	act_animate = RepeatForever::create(Animate::create(anim));
	act_animate->retain();

	//_sprite = Sprite::create();
	//_sprite->setPosition(Vec2::ZERO);
	//_sprite->retain();
	//this->addChild(_sprite);

	//_action = RepeatForever::create(animate);
	//_action->retain();

	//_sprite->runAction(_action);
}

void Fire::initPhysics()
{
	float scale_factor = Director::getInstance()->getContentScaleFactor();

	Size s = Size(SIZE_WIDTH / scale_factor, SIZE_WIDTH / scale_factor);

	_body = PhysicsBody::createBox(s, PhysicsMaterial(0.1F, 0.0f, 0.0f));
	_body->setDynamic(true);
	_body->setGravityEnable(false);
	this->setPhysicsBody(_body);

	// call Projectile's func
	this->setColliderBitMask();
}


//void Fire::onEnable()
//{
//
//}
//
//void Fire::onDisable()
//{
//
//}

/* Reset this object
parameter: x, y, direct, team
// method called by pool*/
void Fire::reset(float params[10])
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

void Fire::attack(Character* target, Character::DIRECTION dir)
{
	// make enemy become hurted
	Projectile::attack(target, dir);

	
	// test
	//auto fx = Effect::createEffect(Effect::EFFECT_TYPE::FIRE_EXPLODE_SMALL, this->getPositionX(), this->getPositionY(), 3);
	//fx->setActive(false);

	//int par[10] = { this->getPositionX(), this->getPositionY() , 1, Effect::EFFECT_TYPE::FIRE_EXPLODE_SMALL , 1, 1, 0,0,0,0 };

	//fx->reset(par);

	/*auto fx = MAKE_EFFECT(Effect::EFFECT_TYPE::FIRE_EXPLODE_SMALL, this->getPositionX(), this->getPositionY(), 1, 1, 1);
	World::getInstance()->addChild(fx, World::WORLD_LAYER::EFFECT_FRONT);*/
	this->onDie();


}

void Fire::onDie()
{
	int id = AUDIO::play2d("fire_crash.ogg");
	AUDIO::setVolume(id, 0.5f);
	SPAWN_EFFECT(Effect::EFFECT_TYPE::FIRE_EXPLODE_SMALL, this->getPositionX(), this->getPositionY(), 1, 1, 1, World::WORLD_LAYER::EFFECT_FRONT);

	this->setActive(false);
}