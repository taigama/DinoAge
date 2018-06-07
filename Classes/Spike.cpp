#include "Spike.h"

#include "Projectile.h"

#define SPIKE_SPEED 0.0f
#define SPIKE_DAMAGE 5.0f
#define SPIKE_FORCE_FACTOR 1.0f

USING_NS_CC;

Spike::~Spike()
{

}

bool Spike::init()
{
	// Super init()
	if (!Projectile::init())
	{
		return false;
	}

	// Sets properties
	_speed = SPIKE_SPEED;
	_damage = SPIKE_DAMAGE;
	_force = SPIKE_FORCE_FACTOR;
	_projectileType = Projectile::PROJECTILE_TYPE::SPIKE_ATTACK;
	_team = 2;

	return true;
}

void Spike::setPhysics()
{
	// ------------------------------------------------------------
	// ONLY CALL AFTER SETTING CONTENT SIZE FOR "THIS" PROJECTILE
	// ------------------------------------------------------------

	_body = PhysicsBody::createBox(getContentSize(), PhysicsMaterial(0.1f, 0.0f, 0.5f));
	_body->setDynamic(false);

	// Sets owner for physics body
	this->setPhysicsBody(_body);

	setColliderBitMask();
}

void Spike::attack(Character* target, Character::DIRECTION direct)
{
	Projectile::attack(target, direct);

	// NO effects needed for this Projectile
}

