#include "Charge.h"

#include "Projectile.h"

#define CHARGE_FORCE 1.5f;
#define CHARGE_SPEED 0;
#define CHARGE_DAMAGE 20;

USING_NS_CC;

Charge::~Charge()
{

}

bool Charge::init()
{
	// Super init()
	if (!Projectile::init())
	{
		return false;
	}

	// Sets values for properties
	_speed = CHARGE_SPEED;
	_damage = CHARGE_DAMAGE;
	_force = CHARGE_FORCE;

	_projectileType = PROJECTILE_TYPE::CHARGE_ATTACK;
	_team = 2;

	return true;
}

void Charge::setPhysics()
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

void Charge::attack(Character* target, Character::DIRECTION direct)
{
	Projectile::attack(target, direct);

	// Effects
}
