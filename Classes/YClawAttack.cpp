#include "YClawAttack.h"

#include "MyPoolManager.h"
#include "WorldScene.h"

//#include "Effect.h"// test
//#include "WorldScene.h"

USING_NS_CC;

#define ATTACK_TIME 0.25f
#define DAMAGE 10

#define SIZE_WIDTH 15
#define SIZE_HEIGHT 30

bool YClawAttack::init()
{
	if (!Projectile::init())
		return false;
	// sprite init & physic init were called in base

	_projectileType = PROJECTILE_TYPE::YCLAW_ATTACK;

	_damage = DAMAGE;

	return true;
}

void YClawAttack::update(float delta)
{
	if (_timer <= 0)
	{
		this->setActive(false);
	}
	_timer -= delta;
}

void YClawAttack::initPhysics()
{
	float scale_factor = Director::getInstance()->getContentScaleFactor();
	Size s = Size(SIZE_WIDTH / scale_factor, SIZE_HEIGHT / scale_factor);

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
void YClawAttack::reset(float params[10])
{
	this->setActive(true);
	_timer = ATTACK_TIME;

	this->setPosition( _host->convertToNodeSpace(Vec2(params[0], params[1])));

	_team = params[3];

}

void YClawAttack::attack(Character* target, Character::DIRECTION dir)
{
	// make enemy hurted
	Projectile::attack(target, dir);
	Vec2 pos = _host->convertToWorldSpace(_position);


	int rand = random<int>(0, 1);
	if (rand == 0)
	{
		SPAWN_EFFECT(Effect::EFFECT_TYPE::SPARK_RED_I
			, pos.x
			, pos.y
			, _scaleX
			, 1
			, 1//scale
			, World::WORLD_LAYER::EFFECT_FRONT);
	}
	else// if rand == 1
	{
		SPAWN_EFFECT(Effect::EFFECT_TYPE::SPARK_RED_II
			, pos.x
			, pos.y
			, _scaleX
			, 1
			, 1//scale
			, World::WORLD_LAYER::EFFECT_FRONT);
	}
}
