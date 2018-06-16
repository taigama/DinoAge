#include "DragonNova.h"


//#include "Effect.h"// test
//#include "WorldScene.h"

USING_NS_CC;

#define ATTACK_TIME 0.1f
#define DAMAGE 100.0f

#define SIZE_WIDTH 854.0f
#define SIZE_HEIGHT 480.0f

bool DragonNova::init()
{
	if (!Projectile::init())
		return false;
	// sprite init & physic init were called in base

	_projectileType = PROJECTILE_TYPE::DRAGON_NOVA;

	_damage = DAMAGE;

	return true;
}

void DragonNova::update(float delta)
{	
	if (_timer <= 0)
	{
		this->setActive(false);
	}
	_timer -= delta;
}


void DragonNova::initPhysics()
{
	float scale_factor = Director::getInstance()->getContentScaleFactor();
	Size s = Size(SIZE_WIDTH / scale_factor, SIZE_HEIGHT / scale_factor);

	_body = PhysicsBody::createBox(s, PhysicsMaterial(0.1F, 0.0f, 0.0f));
	_body->setDynamic(false);
	this->setPhysicsBody(_body);

	// call Projectile's func
	this->setColliderBitMask();
}

/* Reset this object
parameter: x, y, direct, team
// method called by pool*/
void DragonNova::reset(float params[10])
{
	this->setActive(true);
	_timer = ATTACK_TIME;
	this->scheduleUpdate();

	this->setPosition(Vec2(params[0], params[1]));

	_team = params[3];
}