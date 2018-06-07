#include "SelfExplode.h"

#include "EnemyFeeder.h"

#define SELF_EXPLODE_SPEED 0.0f

#define SELF_EXPLODE_DAMAGE_SMALL 10.0f
#define SELF_EXPLODE_DAMAGE_LARGE 30.0f

#define SELF_EXPLODE_FORCE_SMALL 1.0f
#define SELF_EXPLODE_FORCE_LARGE 2.0f

USING_NS_CC;


SelfExplode::~SelfExplode()
{

}

bool SelfExplode::init()
{
	// Super init
	if (!Projectile::init())
	{
		return false;
	}


	// Sets values for properties
	_speed = SELF_EXPLODE_SPEED;
	_damage = SELF_EXPLODE_DAMAGE_SMALL;	// default
	_force = SELF_EXPLODE_FORCE_SMALL;		// default

	_projectileType = Projectile::PROJECTILE_TYPE::SELF_EXPLODE;
	_team = 2;		// enemy team

	return true;
}

void SelfExplode::setPhysics()
{
	// ==========================================
	// ONLY CALLED AFTER SETTING EXPLOSION TYPE
	// ==========================================

	auto scaleFactor = Director::getInstance()->getContentScaleFactor();


	// Sets physics body
	_body = PhysicsBody::createBox(getContentSize(), PhysicsMaterial(0.0f, 0.0f, 0.0f));
	_body->setDynamic(false);

	this->setPhysicsBody(_body);

	setColliderBitMask();
}

void SelfExplode::setExplosionType(int size)
{
	if (size == (int)EnemyFeeder::FEEDER_SIZE::LARGE)
	{
		_damage = SELF_EXPLODE_DAMAGE_LARGE;
		_force = SELF_EXPLODE_FORCE_LARGE;
	}

	// Sets content size
	this->setContentSize(_host->getContentSize() * 2.8f);
}

void SelfExplode::attack(Character* target, Character::DIRECTION direction)
{
	// Super
	Projectile::attack(target, direction);
}
