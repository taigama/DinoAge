#include "LightningStrike.h"
#include "MyPoolManager.h"
#include "WorldScene.h"

USING_NS_CC;

#define LIGHTNING_SIZE_HEIGHT 100
#define LIGHTNING_SIZE_WIDTH 10

#define LIGHTNING_SIZE_BASE_HEIGHT 20
#define LIGHTNING_SIZE_BASE_WIDTH 30
#define LIGHTNING_OFFSET_BASE_Y -40

#define LIGHTNING_DAMAGE 25.0f
#define LIGHTNING_TIMER 0.3f

LightningStrike::~LightningStrike() {}

bool LightningStrike::init()
{
	if (!Projectile::init())
		return false;
	// sprite init & physic init were called in base

	_projectileType = PROJECTILE_TYPE::LIGHTNING_STRIKE;
	_damage = LIGHTNING_DAMAGE;


	
	return true;
}

void LightningStrike::update(float delta)
{
	if (_timer <= 0)
	{
		this->setActive(false);
	}


	_timer -= delta;
}

void LightningStrike::initPhysics()
{
	float scale_factor = Director::getInstance()->getContentScaleFactor();

	

	_body = PhysicsBody::create();

	Size s = Size(LIGHTNING_SIZE_WIDTH / scale_factor, LIGHTNING_SIZE_HEIGHT / scale_factor);	
	// will overlap, make double dmg at the center (I like this)
	_body->addShape(PhysicsShapeBox::create(s));

	// you want the above shape no overlap with the following? use this instead the line above
	//_body->addShape(PhysicsShapeBox::create(s, PHYSICSSHAPE_MATERIAL_DEFAULT, Vec2(0, (s.height / 2 + LIGHTNING_OFFSET_BASE_Y + LIGHTNING_SIZE_BASE_HEIGHT/2) / scale_factor)));

	s.width = LIGHTNING_SIZE_BASE_WIDTH / scale_factor;
	s.height = LIGHTNING_SIZE_BASE_HEIGHT / scale_factor;	
	_body->addShape(PhysicsShapeBox::create(s, PHYSICSSHAPE_MATERIAL_DEFAULT, Vec2(0, LIGHTNING_OFFSET_BASE_Y / scale_factor)));

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
void LightningStrike::reset(float params[10])
{
	this->setActive(true);

	this->setPosition(Vec2(params[0], params[1]));
	this->setScaleX(params[2]);

	scheduleUpdate();

	_team = params[3];

	_timer = LIGHTNING_TIMER;

	SPAWN_EFFECT(Effect::EFFECT_TYPE::LIGHTNING_STRIKE, params[0], params[1], 1, 1, 0.5f, World::WORLD_LAYER::EFFECT_FRONT);
}