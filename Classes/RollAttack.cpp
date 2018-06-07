#include "RollAttack.h"

#define ROLL_ATTACK_SPEED 0.0f
#define ROLL_ATTACK_DAMAGE 5.0f
#define ROLL_ATTACK_FORCE_FACTOR 1.5f

USING_NS_CC;


RollAttack::~RollAttack()
{

}

bool RollAttack::init()
{
	// Super init()
	if (!Projectile::init())
	{
		return false;
	}

	// Sets properties
	_speed = ROLL_ATTACK_SPEED;
	_damage = ROLL_ATTACK_DAMAGE;
	_force = ROLL_ATTACK_FORCE_FACTOR;

	_projectileType = Projectile::PROJECTILE_TYPE::ROLL_ATTACK;
	_team = 2;		// will be override by host's team anyway

	return true;
}

void RollAttack::setPhysics()
{
	// ------------------------------------------------------------
	// ONLY CALL AFTER SETTING CONTENT SIZE FOR "THIS" PROJECTILE
	// ------------------------------------------------------------

	// Initializes PhysicsBody based on the content size that
	// was set earlier by the EnemyRamus possessing this RollAttack
	_body = PhysicsBody::createBox(getContentSize(), PhysicsMaterial(0.1f, 0.0f, 0.5f));
	_body->setDynamic(false);

	// Sets owner
	this->setPhysicsBody(_body);

	// Sets all bitmasks
	setColliderBitMask();
}

void RollAttack::attack(Character* target, Character::DIRECTION direct)
{
	// ALWAYS calls super attack with DIRECTION:BOTTOM
	Projectile::attack(target, Character::DIRECTION::BOTTOM);

	// NO effects needed for this Projectile
}
